/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

/*!
 * \file kernel_operator_data_copy_check.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_data_copy_check.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_data_copy_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_CHECK_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_DATA_COPY_CHECK_H
#define ASCENDC_MODULE_OPERATOR_DATA_COPY_CHECK_H

#include "kernel_npu_debug.h"

namespace AscendC {
/* **************************************************************************************************
 * DataCopy                                             *
 * ************************************************************************************************* */
template <typename T>
__aicore__ inline constexpr bool IsBasicDataCopySupportedType()
{
    return SupportType<
               PrimT<T>, int4b_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, half, float,
               double>()
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
           || SupportType<PrimT<T>, bfloat16_t>()
#endif
        ;
}

template <typename T>
__aicore__ inline constexpr bool IsNd2NzSupportedType()
{
    return SupportType<PrimT<T>, int4b_t, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, half, float>()
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 2201)
           || SupportType<PrimT<T>, bfloat16_t>()
#endif
        ;
}

template <typename T>
__aicore__ inline constexpr bool IsDataCopyPadSupportedType()
{
    return IsBasicDataCopySupportedType<T>();
}

template <typename T>
__aicore__ inline void CheckBasicDataCopyTypeSupport(const __gm__ char* apiName)
{
    ASCENDC_DEBUG_ASSERT(
        true,
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed to check dtype support in %s, supported dtypes are int4b_t/int8_t/uint8_t/int16_t/uint16_t/int32_t/"
            "uint32_t/int64_t/uint64_t/half/bfloat16_t/float/double.\n",
            apiName));
}

template <typename T>
__aicore__ inline void CheckNd2NzTypeSupport(const __gm__ char* apiName)
{
    ASCENDC_DEBUG_ASSERT(
        true,
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed to check dtype support in %s, supported dtypes are int4b_t/int8_t/uint8_t/int16_t/uint16_t/int32_t/"
            "uint32_t/half/bfloat16_t/float.\n",
            apiName));
}

template <typename T>
__aicore__ inline void CheckDataCopyPadTypeSupport(const __gm__ char* apiName)
{
    ASCENDC_DEBUG_ASSERT(
        true,
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed to check dtype support in %s, supported dtypes are int4b_t/half/bfloat16_t/int16_t/uint16_t/float/"
            "int32_t/uint32_t/int8_t/uint8_t/int64_t/uint64_t/double.\n",
            apiName));
}

template <typename T, typename U>
__aicore__ inline void CheckBasicDataCopyMixedTypeSupport(const __gm__ char* apiName)
{
    ASCENDC_DEBUG_ASSERT(
        true, KERNEL_LOG_INTERNAL(
                  KERNEL_ERROR,
                  "Failed to check dtype support in %s, supported "
                  "dtype combinations are same primitive dtype or half -> float.\n",
                  apiName));
}

__aicore__ inline void CheckDataCopyParamsCommon(const DataCopyParams& intriParams, const __gm__ char* apiName)
{
    CheckValueRange<uint16_t>(intriParams.blockCount, 0, UINT12_MAX, "blockCount", apiName);
    CheckValueRange<uint16_t>(intriParams.blockLen, 0, UINT16_MAX, "blockLen", apiName);
    ReportNopWarning<uint16_t>(intriParams.blockCount, "repeatParams.blockCount", apiName);
    ReportNopWarning<uint16_t>(intriParams.blockLen, "repeatParams.blockLen", apiName);
}

__aicore__ inline void CheckDataCopyLocalRoute(
    const Hardware srcHwPos, const Hardware dstHwPos, const TPosition srcPos, const TPosition dstPos,
    const __gm__ char* apiName)
{
    if (srcHwPos == Hardware::UB) {
        ASCENDC_DEBUG_ASSERT(
            (dstHwPos == Hardware::UB || dstHwPos == Hardware::L1),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check dst tensor route in %s, supported routes are UB(VECIN)->UB(VECCALC) / "
                "UB(VECCALC)->UB(VECOUT) / UB(VECIN/VECCALC/VECOUT)->L1 Buffer(TSCM), current dst position is "
                "%s%s.\n",
                apiName, GetHardwareName(dstHwPos), GetTPositionSuffix(dstPos)));
        return;
    }
    if (srcHwPos == Hardware::L1) {
        const bool isValidDst = (dstHwPos == Hardware::BIAS || dstHwPos == Hardware::FIXBUF);
        ASCENDC_DEBUG_ASSERT(
            (isValidDst), KERNEL_LOG_INTERNAL(
                              KERNEL_ERROR,
                              "Failed to check dst tensor route in "
                              "%s, supported routes are L1 Buffer(A1/B1/C1)->Fixpipe Buffer(C2PIPE2GM) / "
                              "L1 Buffer(C1)->BiasTable Buffer(C2), current dst position is %s%s.\n",
                              apiName, GetHardwareName(dstHwPos), GetTPositionSuffix(dstPos)));
        return;
    }
    ASCENDC_DEBUG_ASSERT(
        (false), KERNEL_LOG_INTERNAL(
                     KERNEL_ERROR,
                     "Failed to check src tensor route in %s, "
                     "supported src positions are UB(VECIN/VECCALC/VECOUT)/L1 Buffer(A1/B1/C1), current src "
                     "position is %s%s.\n",
                     apiName, GetHardwareName(srcHwPos), GetTPositionSuffix(srcPos)));
}

template <typename T>
__aicore__ inline void CheckDataCopyLocalAlignment(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const Hardware srcHwPos, const Hardware dstHwPos,
    const __gm__ char* apiName)
{
    CheckTensorAlignment(src, ONE_BLK_SIZE, "src", apiName);
    if (srcHwPos != Hardware::L1) {
        CheckTensorAlignment(dst, ONE_BLK_SIZE, "dst", apiName);
        return;
    }
    if (dstHwPos == Hardware::BIAS) {
        CheckTensorAlignment(dst, 64, "dst", apiName);
        return;
    }
#if (__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3102) || (__NPU_ARCH__ == 3510) || \
    (__NPU_ARCH__ == 5102)
    if (dstHwPos == Hardware::FIXBUF) {
        CheckTensorAlignment(dst, 128, "dst", apiName);
        return;
    }
#endif
    CheckTensorAlignment(dst, ONE_BLK_SIZE, "dst", apiName);
}

template <typename T>
__aicore__ inline void CheckDataCopyTensor(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyParams& intriParams,
    const __gm__ char* apiName)
{
    CheckDataCopyParamsCommon(intriParams, apiName);
    CheckTensorPhyPosition<Hardware::L1, Hardware::UB>(dst, "dst", "L1 Buffer(A1/B1/C1)/UB(VECIN)", apiName);
    CheckTensorAlignment(dst, ONE_BLK_SIZE, "dst", apiName);
}

template <typename T>
__aicore__ inline void CheckDataCopyTensor(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& intriParams,
    const __gm__ char* apiName)
{
    (void)dst;
    CheckDataCopyParamsCommon(intriParams, apiName);
    CheckTensorPhyPosition<Hardware::L1, Hardware::UB>(src, "src", "L1 Buffer(A1/B1)/UB(VECOUT)", apiName);
    CheckTensorAlignment(src, ONE_BLK_SIZE, "src", apiName);
}

template <typename T>
__aicore__ inline void CheckDataCopyTensor(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& intriParams, const __gm__ char* apiName)
{
    const TPosition dstPos = static_cast<TPosition>(dst.GetPosition());
    const TPosition srcPos = static_cast<TPosition>(src.GetPosition());
    const Hardware dstHwPos = GetPhyType(dstPos);
    const Hardware srcHwPos = GetPhyType(srcPos);
    CheckDataCopyParamsCommon(intriParams, apiName);
    CheckDataCopyLocalRoute(srcHwPos, dstHwPos, srcPos, dstPos, apiName);
    CheckDataCopyLocalAlignment(dst, src, srcHwPos, dstHwPos, apiName);
}

template <typename T, typename U>
__aicore__ inline void CheckDataCopyTensor(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& intriParams, const __gm__ char* apiName)
{
    const TPosition dstPos = static_cast<TPosition>(dst.GetPosition());
    const TPosition srcPos = static_cast<TPosition>(src.GetPosition());
    const Hardware dstHwPos = GetPhyType(dstPos);
    const Hardware srcHwPos = GetPhyType(srcPos);

    CheckDataCopyParamsCommon(intriParams, apiName);
    ASCENDC_DEBUG_ASSERT(
        (srcHwPos == Hardware::L1), KERNEL_LOG_INTERNAL(
                                        KERNEL_ERROR,
                                        "Failed to check src tensor "
                                        "route in %s, supported route is L1 Buffer(C1)->BiasTable Buffer(C2), "
                                        "current src position is %s%s.\n",
                                        apiName, GetHardwareName(srcHwPos), GetTPositionSuffix(srcPos)));
    ASCENDC_DEBUG_ASSERT(
        (dstHwPos == Hardware::BIAS), KERNEL_LOG_INTERNAL(
                                          KERNEL_ERROR,
                                          "Failed to check dst tensor "
                                          "route in %s, supported route is L1 Buffer(C1)->BiasTable Buffer(C2), "
                                          "current dst position is %s%s.\n",
                                          apiName, GetHardwareName(dstHwPos), GetTPositionSuffix(dstPos)));
    CheckTensorAlignment(src, ONE_BLK_SIZE, "src", apiName);
    CheckTensorAlignment(dst, 64, "dst", apiName);
}

__aicore__ inline void CheckNd2NzParamsCommon(const Nd2NzParams& intriParams, const __gm__ char* apiName)
{
    CheckValueRange<decltype(intriParams.ndNum)>(intriParams.ndNum, 0, UINT12_MAX, "ndNum", apiName);
    CheckValueRange<decltype(intriParams.nValue)>(intriParams.nValue, 0, 16384, "nValue", apiName);
    CheckValueRange<decltype(intriParams.dValue)>(intriParams.dValue, 0, 65535, "dValue", apiName);
    CheckValueRange<decltype(intriParams.srcNdMatrixStride)>(
        intriParams.srcNdMatrixStride, 0, 65535, "srcNdMatrixStride", apiName);
    CheckValueRange<decltype(intriParams.srcDValue)>(intriParams.srcDValue, 1, 65535, "srcDValue", apiName);
    CheckValueRange<decltype(intriParams.dstNzC0Stride)>(intriParams.dstNzC0Stride, 1, 16384, "dstNzC0Stride", apiName);
    CheckValueRange<decltype(intriParams.dstNzNStride)>(intriParams.dstNzNStride, 1, 16384, "dstNzNStride", apiName);
    CheckValueRange<decltype(intriParams.dstNzMatrixStride)>(
        intriParams.dstNzMatrixStride, 0, 65535, "dstNzMatrixStride", apiName);

    ReportNopWarning<uint16_t>(intriParams.ndNum, "intriParams.ndNum", apiName);
    ReportNopWarning<uint16_t>(intriParams.nValue, "intriParams.nValue", apiName);
    ReportNopWarning<uint16_t>(intriParams.dValue, "intriParams.dValue", apiName);
}

__aicore__ inline void CheckNz2NdParamsCommon(const Nz2NdParamsFull& intriParams, const __gm__ char* apiName)
{
    constexpr uint16_t nz2NdLimit = 8192;
    CheckValueRange<decltype(intriParams.ndNum)>(intriParams.ndNum, 0, UINT12_MAX, "ndNum", apiName);
    CheckValueRange<decltype(intriParams.nValue)>(intriParams.nValue, 1, nz2NdLimit, "nValue", apiName);
    CheckValueRange<decltype(intriParams.dValue)>(intriParams.dValue, 1, nz2NdLimit, "dValue", apiName);
    CheckValueRange<decltype(intriParams.srcNdMatrixStride)>(
        intriParams.srcNdMatrixStride, 1, VALUE_512, "srcNdMatrixStride", apiName);
    CheckValueRange<decltype(intriParams.srcNStride)>(intriParams.srcNStride, 0, UINT12_MAX, "srcNStride", apiName);
}

template <typename T>
__aicore__ inline void CheckNd2NzRoute(
    const Hardware srcHwPos, const Hardware dstHwPos, const TPosition srcPos, const TPosition dstPos,
    const __gm__ char* apiName)
{
    ASCENDC_DEBUG_ASSERT(
        (srcHwPos == Hardware::UB),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed to check src tensor "
            "position in %s, supported positions are UB(VECIN/VECCALC/VECOUT), current position is %s%s.\n",
            apiName, GetHardwareName(srcHwPos), GetTPositionSuffix(srcPos)));
    ASCENDC_DEBUG_ASSERT(
        (dstHwPos == Hardware::L1), KERNEL_LOG_INTERNAL(
                                        KERNEL_ERROR,
                                        "Failed to check dst tensor "
                                        "position in %s, supported positions are L1 Buffer(TSCM), current position is "
                                        "%s%s.\n",
                                        apiName, GetHardwareName(dstHwPos), GetTPositionSuffix(dstPos)));
}

__aicore__ inline void CheckUbToL1Route(
    const Hardware srcHwPos, const Hardware dstHwPos, const TPosition srcPos, const TPosition dstPos,
    const __gm__ char* apiName)
{
    ASCENDC_DEBUG_ASSERT(
        (srcHwPos == Hardware::UB),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed to check src tensor "
            "route in %s, supported route is UB(VECIN/VECOUT)->L1 Buffer(TSCM), current src position is %s%s.\n",
            apiName, GetHardwareName(srcHwPos), GetTPositionSuffix(srcPos)));
    ASCENDC_DEBUG_ASSERT(
        (dstHwPos == Hardware::L1),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed to check dst tensor "
            "route in %s, supported route is UB(VECIN/VECOUT)->L1 Buffer(TSCM), current dst position is %s%s.\n",
            apiName, GetHardwareName(dstHwPos), GetTPositionSuffix(dstPos)));
}

template <typename T>
__aicore__ inline void CheckDataCopyTensor(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams, const __gm__ char* apiName)
{
    CheckNd2NzParamsCommon(intriParams, apiName);
    CheckTensorPhyPosition<Hardware::L1, Hardware::UB>(dst, "dst", "L1 Buffer(A1/B1)/UB(VECIN)", apiName);
    CheckTensorAlignment(dst, ONE_BLK_SIZE, "dst", apiName);
}

template <typename T>
__aicore__ inline void CheckDataCopyTensor(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const Nd2NzParams& intriParams, const __gm__ char* apiName)
{
    const TPosition dstPos = static_cast<TPosition>(dst.GetPosition());
    const TPosition srcPos = static_cast<TPosition>(src.GetPosition());
    const Hardware dstHwPos = GetPhyType(dstPos);
    const Hardware srcHwPos = GetPhyType(srcPos);
    CheckNd2NzParamsCommon(intriParams, apiName);
    CheckNd2NzRoute<T>(srcHwPos, dstHwPos, srcPos, dstPos, apiName);
    CheckTensorAlignment(src, ONE_BLK_SIZE, "src", apiName);
    CheckTensorAlignment(dst, ONE_BLK_SIZE, "dst", apiName);
}

template <typename T>
__aicore__ inline void CheckDataCopyTensor(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const Nz2NdParamsFull& intriParams,
    const __gm__ char* apiName)
{
    (void)dst;
    const TPosition srcPos = static_cast<TPosition>(src.GetPosition());
    const Hardware srcHwPos = GetPhyType(srcPos);

    CheckNz2NdParamsCommon(intriParams, apiName);
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    const bool isValidSrc = (srcHwPos == Hardware::UB || srcHwPos == Hardware::L1);
    const __gm__ char* supportedPos = "UB(VECOUT)/L1 Buffer(A1/B1)";
#else
    const bool isValidSrc = (srcHwPos == Hardware::UB);
    const __gm__ char* supportedPos = "UB(VECOUT)";
#endif
    ASCENDC_DEBUG_ASSERT(
        (isValidSrc), KERNEL_LOG_INTERNAL(
                          KERNEL_ERROR,
                          "Failed to check src tensor route in %s, "
                          "supported src positions are %s, current src position is %s%s.\n",
                          apiName, supportedPos, GetHardwareName(srcHwPos), GetTPositionSuffix(srcPos)));
    ASCENDC_DEBUG_ASSERT(
        (intriParams.dValue % BLOCK_CUBE == 0),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed to "
            "check dValue value in %s, dValue must be divisible by %u, current value is %u.\n",
            apiName, static_cast<uint32_t>(BLOCK_CUBE), intriParams.dValue));
    CheckTensorAlignment(src, ONE_BLK_SIZE, "src", apiName);
}

template <typename T>
__aicore__ inline void CheckDataCopyPadParamsCommon(
    const DataCopyParams& dataCopyParams, const bool isGmToUb, const __gm__ char* apiName)
{
    constexpr uint32_t dataCopyPadBlkLenLimit = 2097151;
    CheckValueRange<uint16_t>(dataCopyParams.blockCount, 0, UINT12_MAX, "blockCount", apiName);
    CheckValueRange<uint32_t>(
        static_cast<uint32_t>(dataCopyParams.blockLen), 0, dataCopyPadBlkLenLimit, "blockLen", apiName);
    ReportNopWarning<uint16_t>(dataCopyParams.blockCount, "dataCopyParams.blockCount", apiName);
    ReportNopWarning<uint16_t>(dataCopyParams.blockLen, "dataCopyParams.blockLen", apiName);

    if (isGmToUb) {
        ASCENDC_DEBUG_ASSERT(
            (dataCopyParams.blockLen % sizeof(PrimT<T>) == 0),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check DataCopyParams.blockLen value in %s, it must be divisible by %u, current "
                "value is %u.\n",
                apiName, sizeof(PrimT<T>), dataCopyParams.blockLen));
    }
}

template <typename T>
__aicore__ inline void CheckDataCopyPadParamsCommon(
    const DataCopyExtParams& dataCopyParams, const bool isGmToUb, const __gm__ char* apiName)
{
    constexpr uint32_t dataCopyPadBlkLenLimit = 2097151;
    CheckValueRange<uint16_t>(dataCopyParams.blockCount, 0, UINT12_MAX, "blockCount", apiName);
    CheckValueRange<uint32_t>(dataCopyParams.blockLen, 0, dataCopyPadBlkLenLimit, "blockLen", apiName);
    ReportNopWarning<uint16_t>(dataCopyParams.blockCount, "dataCopyParams.blockCount", apiName);
    ReportNopWarning<uint32_t>(dataCopyParams.blockLen, "dataCopyParams.blockLen", apiName);

    if (isGmToUb) {
        ASCENDC_DEBUG_ASSERT(
            (dataCopyParams.blockLen % sizeof(PrimT<T>) == 0),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check DataCopyExtParams.blockLen value in %s, it must be divisible by %u, current "
                "value is %u.\n",
                apiName, sizeof(PrimT<T>), dataCopyParams.blockLen));
    }
}

template <typename T, typename PadParamsT>
__aicore__ inline void CheckDataCopyPadPaddingCommon(const PadParamsT& padParams, const __gm__ char* apiName)
{
    constexpr uint32_t padLimit = ONE_BLK_SIZE;
    ASCENDC_DEBUG_ASSERT(
        (static_cast<uint32_t>(padParams.leftPadding) * sizeof(PrimT<T>) <= padLimit),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed to check leftPadding value in %s, leftPadding bytes must be less "
            "than or equal to %u, current bytes is %u.\n",
            apiName, padLimit, static_cast<uint32_t>(padParams.leftPadding) * sizeof(PrimT<T>)));
    ASCENDC_DEBUG_ASSERT(
        (static_cast<uint32_t>(padParams.rightPadding) * sizeof(PrimT<T>) <= padLimit),
        KERNEL_LOG_INTERNAL(
            KERNEL_ERROR,
            "Failed to check rightPadding value in %s, rightPadding bytes must be less "
            "than or equal to %u, current bytes is %u.\n",
            apiName, padLimit, static_cast<uint32_t>(padParams.rightPadding) * sizeof(PrimT<T>)));
    if constexpr (Std::is_same_v<PadParamsT, DataCopyPadParams>) {
        // paddingValue is always uint64_t dtype
        ASCENDC_DEBUG_ASSERT(
            (padParams.paddingValue == 0),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check paddingValue value in %s, DataCopyPadParams.paddingValue only supports 0.\n",
                apiName));
    } else if constexpr (sizeof(PrimT<T>) == sizeof(uint64_t)) {
        ASCENDC_DEBUG_ASSERT(
            (Internal::CheckIsZero<T>(padParams.paddingValue)),
            KERNEL_LOG_INTERNAL(
                KERNEL_ERROR,
                "Failed to check paddingValue value in %s, when data type length is 64 bits, paddingValue only "
                "supports "
                "0.\n",
                apiName));
    }
}

template <typename T, typename ParamsT, typename PadParamsT>
__aicore__ inline void CheckGmToLocalPadTensor(
    const LocalTensor<T>& dst, const ParamsT& dataCopyParams, const PadParamsT& padParams, const bool isGmToUb,
    const bool isValidDst, const __gm__ char* apiName)
{
    const TPosition dstPos = static_cast<TPosition>(dst.GetPosition());
    CheckDataCopyPadParamsCommon<T>(dataCopyParams, isGmToUb, apiName);
    CheckDataCopyPadPaddingCommon<T>(padParams, apiName);
    ASCENDC_DEBUG_ASSERT(
        (isValidDst), KERNEL_LOG_INTERNAL(
                          KERNEL_ERROR,
                          "Failed to check dst tensor route in %s, "
                          "supported route is GM->UB(VECIN/VECOUT), current dst position is %s%s.\n",
                          apiName, GetHardwareName(GetPhyType(dstPos)), GetTPositionSuffix(dstPos)));
    CheckTensorAlignment(dst, ONE_BLK_SIZE, "dst", apiName);
}

template <typename T, typename ParamsT>
__aicore__ inline void CheckLocalToGmPadTensor(
    const LocalTensor<T>& src, const ParamsT& dataCopyParams, const bool isValidSrc, const __gm__ char* apiName)
{
    const TPosition srcPos = static_cast<TPosition>(src.GetPosition());
    CheckDataCopyPadParamsCommon<T>(dataCopyParams, false, apiName);
    ASCENDC_DEBUG_ASSERT(
        (isValidSrc), KERNEL_LOG_INTERNAL(
                          KERNEL_ERROR,
                          "Failed to check src tensor route in %s, "
                          "supported route is UB(VECIN/VECOUT)->GM, current src position is %s%s.\n",
                          apiName, GetHardwareName(GetPhyType(srcPos)), GetTPositionSuffix(srcPos)));
    CheckTensorAlignment(src, ONE_BLK_SIZE, "src", apiName);
}

template <typename T, typename ParamsT>
__aicore__ inline void CheckPadUbToL1Tensor(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const ParamsT& dataCopyParams, const Nd2NzParams& nd2nzParams,
    const __gm__ char* apiName)
{
    const TPosition dstPos = static_cast<TPosition>(dst.GetPosition());
    const TPosition srcPos = static_cast<TPosition>(src.GetPosition());
    const Hardware dstHwPos = GetPhyType(dstPos);
    const Hardware srcHwPos = GetPhyType(srcPos);

    CheckDataCopyPadParamsCommon<T>(dataCopyParams, false, apiName);
    CheckNd2NzParamsCommon(nd2nzParams, apiName);
    ASCENDC_DEBUG_ASSERT(
        (nd2nzParams.ndNum == 1), KERNEL_LOG_INTERNAL(
                                      KERNEL_ERROR,
                                      "Failed to check ndNum value in "
                                      "%s, ndNum only supports 1, current value is %u.\n",
                                      apiName, nd2nzParams.ndNum));
    CheckUbToL1Route(srcHwPos, dstHwPos, srcPos, dstPos, apiName);
    CheckTensorAlignment(src, ONE_BLK_SIZE, "src", apiName);
    CheckTensorAlignment(dst, ONE_BLK_SIZE, "dst", apiName);
}

template <typename T>
__aicore__ inline void CheckDataCopyPadTensor(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyParams& dataCopyParams,
    const DataCopyPadParams& padParams, const __gm__ char* apiName)
{
    (void)src;
    const Hardware dstHwPos = GetPhyType(static_cast<TPosition>(dst.GetPosition()));
    const bool isValidDst = (dstHwPos == Hardware::UB);
    const bool isGmToUb = (dstHwPos == Hardware::UB);
    CheckGmToLocalPadTensor(dst, dataCopyParams, padParams, isGmToUb, isValidDst, apiName);
}

template <typename T>
__aicore__ inline void CheckDataCopyPadTensor(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& dataCopyParams,
    const __gm__ char* apiName)
{
    (void)dst;
    CheckDataCopyPadLocalToGlobalTensor(src, dataCopyParams, apiName);
}

template <typename T, typename U>
__aicore__ inline void CheckDataCopyPadTensor(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyExtParams& dataCopyParams,
    const DataCopyPadExtParams<U>& padParams, const __gm__ char* apiName)
{
    (void)src;
    CheckGmToLocalPadTensor(
        dst, dataCopyParams, padParams, true, GetPhyType(static_cast<TPosition>(dst.GetPosition())) == Hardware::UB,
        apiName);
}

template <typename T, typename CopyParamsT>
__aicore__ inline void CheckDataCopyPadLocalToGlobalTensor(
    const LocalTensor<T>& src, const CopyParamsT& dataCopyParams, const __gm__ char* apiName)
{
    const Hardware srcHwPos = GetPhyType(static_cast<TPosition>(src.GetPosition()));
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    const bool isValidSrc = (srcHwPos == Hardware::UB || srcHwPos == Hardware::L1);
#else
    const bool isValidSrc = (srcHwPos == Hardware::UB);
#endif
    CheckLocalToGmPadTensor(src, dataCopyParams, isValidSrc, apiName);
}

template <typename T>
__aicore__ inline void CheckDataCopyPadTensor(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams& dataCopyParams,
    const __gm__ char* apiName)
{
    (void)dst;
    CheckDataCopyPadLocalToGlobalTensor(src, dataCopyParams, apiName);
}

template <typename T>
__aicore__ inline void CheckDataCopyPadTensor(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& dataCopyParams,
    const Nd2NzParams& nd2nzParams, const __gm__ char* apiName)
{
    CheckPadUbToL1Tensor(dst, src, dataCopyParams, nd2nzParams, apiName);
}

template <typename T>
__aicore__ inline void CheckDataCopyPadTensor(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams& dataCopyParams,
    const Nd2NzParams& nd2nzParams, const __gm__ char* apiName)
{
    CheckPadUbToL1Tensor(dst, src, dataCopyParams, nd2nzParams, apiName);
}

} // namespace AscendC

#endif // ASCENDC_MODULE_OPERATOR_DATA_COPY_CHECK_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_CHECK_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_DATA_COPY_CHECK_H__
#endif
