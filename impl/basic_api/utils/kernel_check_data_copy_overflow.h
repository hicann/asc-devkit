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
 * \file kernel_check_data_copy_overflow.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/utils/kernel_check_data_copy_overflow.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_CHECK_DATA_COPY_OVERFLOW_H__
#endif
#ifndef ASCENDC_MODULE_CHECK_DATA_COPY_OVERFLOW_H
#define ASCENDC_MODULE_CHECK_DATA_COPY_OVERFLOW_H

#if ASCENDC_CPU_DEBUG
#include "../kernel_check_util.h"
#include "../../../include/basic_api/kernel_common.h"
#include "../../../include/basic_api/kernel_struct_data_copy.h"

namespace AscendC {
/* **************************************************************************************************
 * Check function for CPU debug
 * ************************************************************************************************* */
template <typename T>
bool CheckDataCopyTensorSizeOverflow(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyParams& repeatParams)
{
    const Hardware srcPos = Hardware::GM;
    const Hardware dstPos = GetPhyType((TPosition)dst.GetPosition());
    uint64_t srcMaxOffsetBytes = 0;
    uint64_t dstMaxOffsetBytes = 0;
    BlockMode mode = check::GetBlockMode({srcPos, dstPos});
    uint8_t biasConvFlag = 0;
    std::string apiInfo = "DataCopy from " + GetPositionDisplay(TPosition::GM) + " to " +
                          GetPositionDisplay(static_cast<TPosition>(dst.GetPosition()));

    check::CalculateDataCopyMaxOffset<PrimT<T>, PrimT<T>>(
        repeatParams, srcPos, dstPos, mode, srcMaxOffsetBytes, dstMaxOffsetBytes, DeqScale::DEQ_NONE, biasConvFlag);
    return check::ReportTensorSizeOverflow(
        srcPos, dstPos, 0, dst.GetSize() * sizeof(PrimT<T>), srcMaxOffsetBytes, dstMaxOffsetBytes, apiInfo);
}

template <typename T>
bool CheckDataCopyTensorSizeOverflow(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const Nd2NzParams& intriParams)
{
    if (intriParams.ndNum == 0 || intriParams.nValue == 0 || intriParams.dValue == 0) {
        return true;
    }
    const Hardware srcPos = Hardware::GM;
    const Hardware dstPos = GetPhyType((TPosition)dst.GetPosition());
    uint64_t srcMaxOffsetBytes = 0;
    uint64_t dstMaxOffsetBytes = (intriParams.ndNum - 1) * intriParams.dstNzMatrixStride * sizeof(PrimT<T>) +
                                 (intriParams.nValue - 1) * intriParams.dstNzNStride * DEFAULT_C0_SIZE +
                                 (DivCeil(intriParams.dValue * sizeof(PrimT<T>), DEFAULT_C0_SIZE) - 1) *
                                     intriParams.dstNzC0Stride * DEFAULT_C0_SIZE +
                                 DEFAULT_C0_SIZE;
    std::string apiInfo = "DataCopy with Nd2NzParams from " + GetPositionDisplay(TPosition::GM) + " to " +
                          GetPositionDisplay(static_cast<TPosition>(dst.GetPosition()));

    return check::ReportTensorSizeOverflow(
        srcPos, dstPos, 0, dst.GetSize() * sizeof(PrimT<T>), srcMaxOffsetBytes, dstMaxOffsetBytes, apiInfo);
}

template <typename T>
bool CheckDataCopyTensorSizeOverflow(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const Nd2NzParams& intriParams)
{
    if (intriParams.ndNum == 0 || intriParams.nValue == 0 || intriParams.dValue == 0) {
        return true;
    }
    const Hardware srcPos = GetPhyType((TPosition)src.GetPosition());
    const Hardware dstPos = GetPhyType((TPosition)dst.GetPosition());
    uint64_t srcMaxOffsetBytes = ((intriParams.ndNum - 1) * intriParams.srcNdMatrixStride +
                                  (intriParams.nValue - 1) * intriParams.srcDValue + intriParams.dValue) *
                                 sizeof(PrimT<T>);
    uint64_t dstMaxOffsetBytes = (intriParams.ndNum - 1) * intriParams.dstNzMatrixStride * sizeof(PrimT<T>) +
                                 (intriParams.nValue - 1) * intriParams.dstNzNStride * DEFAULT_C0_SIZE +
                                 (DivCeil(intriParams.dValue * sizeof(PrimT<T>), DEFAULT_C0_SIZE) - 1) *
                                     intriParams.dstNzC0Stride * DEFAULT_C0_SIZE +
                                 DEFAULT_C0_SIZE;
    std::string apiInfo = "DataCopy with Nd2NzParams from " +
                          GetPositionDisplay(static_cast<TPosition>(src.GetPosition())) + " to " +
                          GetPositionDisplay(static_cast<TPosition>(dst.GetPosition()));

    return check::ReportTensorSizeOverflow(
        srcPos, dstPos, src.GetSize() * sizeof(PrimT<T>), dst.GetSize() * sizeof(PrimT<T>), srcMaxOffsetBytes,
        dstMaxOffsetBytes, apiInfo);
}

template <typename T>
bool CheckDataCopyTensorSizeOverflow(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
{
    const Hardware srcPos = GetPhyType((TPosition)src.GetPosition());
    const Hardware dstPos = Hardware::GM;
    uint64_t srcMaxOffsetBytes = 0;
    uint64_t dstMaxOffsetBytes = 0;
    BlockMode mode = check::GetBlockMode({srcPos, dstPos});
    uint8_t biasConvFlag = 0;
    std::string apiInfo = "DataCopy from " + GetPositionDisplay(static_cast<TPosition>(src.GetPosition())) + " to " +
                          GetPositionDisplay(TPosition::GM);

    check::CalculateDataCopyMaxOffset<PrimT<T>, PrimT<T>>(
        repeatParams, srcPos, dstPos, mode, srcMaxOffsetBytes, dstMaxOffsetBytes, DeqScale::DEQ_NONE, biasConvFlag);
    return check::ReportTensorSizeOverflow(
        srcPos, dstPos, src.GetSize() * sizeof(PrimT<T>), 0, srcMaxOffsetBytes, dstMaxOffsetBytes, apiInfo);
}

template <typename T>
bool CheckDataCopyTensorSizeOverflow(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams)
{
    const Hardware srcPos = GetPhyType((TPosition)src.GetPosition());
    const Hardware dstPos = GetPhyType((TPosition)dst.GetPosition());
    uint64_t srcMaxOffsetBytes = 0;
    uint64_t dstMaxOffsetBytes = 0;
    BlockMode mode = check::GetBlockMode({srcPos, dstPos});
    uint8_t biasConvFlag = 0;
    std::string apiInfo = "DataCopy from " + GetPositionDisplay(static_cast<TPosition>(src.GetPosition())) + " to " +
                          GetPositionDisplay(static_cast<TPosition>(dst.GetPosition()));

    check::CalculateDataCopyMaxOffset<PrimT<T>, PrimT<T>>(
        repeatParams, srcPos, dstPos, mode, srcMaxOffsetBytes, dstMaxOffsetBytes, DeqScale::DEQ_NONE, biasConvFlag);
    return check::ReportTensorSizeOverflow(
        srcPos, dstPos, src.GetSize() * sizeof(PrimT<T>), dst.GetSize() * sizeof(PrimT<T>), srcMaxOffsetBytes,
        dstMaxOffsetBytes, apiInfo);
}

template <typename T, typename U>
bool CheckDataCopyTensorSizeOverflow(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyParams& repeatParams)
{
    const Hardware srcPos = GetPhyType((TPosition)src.GetPosition());
    const Hardware dstPos = GetPhyType((TPosition)dst.GetPosition());
    uint64_t srcMaxOffsetBytes = 0;
    uint64_t dstMaxOffsetBytes = 0;
    BlockMode mode = check::GetBlockMode({srcPos, dstPos});
    uint8_t biasConvFlag = check::IsBiasConv({srcPos, dstPos}) && (sizeof(PrimT<U>) != sizeof(PrimT<T>));
    std::string apiInfo = "DataCopy from " + GetPositionDisplay(static_cast<TPosition>(src.GetPosition())) + " to " +
                          GetPositionDisplay(static_cast<TPosition>(dst.GetPosition()));

    check::CalculateDataCopyMaxOffset<PrimT<U>, PrimT<T>>(
        repeatParams, srcPos, dstPos, mode, srcMaxOffsetBytes, dstMaxOffsetBytes, DeqScale::DEQ_NONE, biasConvFlag);
    return check::ReportTensorSizeOverflow(
        srcPos, dstPos, src.GetSize() * sizeof(PrimT<U>), dst.GetSize() * sizeof(PrimT<T>), srcMaxOffsetBytes,
        dstMaxOffsetBytes, apiInfo);
}

template <typename T>
bool CheckDataCopyTensorSizeOverflow(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const SliceInfo dstSliceInfo[],
    const SliceInfo srcSliceInfo[], const uint32_t dimValue)
{
    const Hardware srcPos = Hardware::GM;
    const Hardware dstPos = GetPhyType((TPosition)dst.GetPosition());
    uint64_t srcMaxOffsetBytes = 0;
    uint64_t dstMaxOffsetBytes = 1;
    std::string apiInfo = "DataCopy with SliceInfo from " + GetPositionDisplay(TPosition::GM) + " to " +
                          GetPositionDisplay(static_cast<TPosition>(dst.GetPosition()));

    for (uint32_t i = 0; i < dimValue; i++) {
        dstMaxOffsetBytes *= dstSliceInfo[i].shapeValue;
    }
    dstMaxOffsetBytes *= sizeof(PrimT<T>);
    return check::ReportTensorSizeOverflow(
        srcPos, dstPos, 0, dst.GetSize() * sizeof(PrimT<T>), srcMaxOffsetBytes, dstMaxOffsetBytes, apiInfo);
}

template <typename T>
bool CheckDataCopyTensorSizeOverflow(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const SliceInfo dstSliceInfo[],
    const SliceInfo srcSliceInfo[], const uint32_t dimValue)
{
    const Hardware srcPos = GetPhyType((TPosition)src.GetPosition());
    const Hardware dstPos = Hardware::GM;
    uint64_t srcMaxOffsetBytes = 1;
    uint64_t dstMaxOffsetBytes = 0;
    std::string apiInfo = "DataCopy with SliceInfo from " +
                          GetPositionDisplay(static_cast<TPosition>(src.GetPosition())) + " to " +
                          GetPositionDisplay(TPosition::GM);

    for (uint32_t i = 0; i < dimValue; i++) {
        srcMaxOffsetBytes *= dstSliceInfo[i].shapeValue;
    }
    srcMaxOffsetBytes *= sizeof(PrimT<T>);
    return check::ReportTensorSizeOverflow(
        srcPos, dstPos, src.GetSize() * sizeof(PrimT<T>), 0, srcMaxOffsetBytes, dstMaxOffsetBytes, apiInfo);
}

template <typename T>
bool CheckDataCopyTensorSizeOverflow(const LocalTensor<T>& dst, const GlobalTensor<T>& src, const uint32_t count)
{
    const Hardware srcPos = Hardware::GM;
    const Hardware dstPos = GetPhyType((TPosition)dst.GetPosition());
    uint64_t srcMaxOffsetBytes = 0;
    uint64_t dstMaxOffsetBytes = count * sizeof(PrimT<T>);
    std::string apiInfo = "DataCopy from " + GetPositionDisplay(TPosition::GM) + " to " +
                          GetPositionDisplay(static_cast<TPosition>(dst.GetPosition()));

    return check::ReportTensorSizeOverflow(
        srcPos, dstPos, 0, dst.GetSize() * sizeof(PrimT<T>), srcMaxOffsetBytes, dstMaxOffsetBytes, apiInfo);
}

template <typename T>
bool CheckDataCopyTensorSizeOverflow(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
{
    const Hardware srcPos = GetPhyType((TPosition)src.GetPosition());
    const Hardware dstPos = GetPhyType((TPosition)dst.GetPosition());
    uint64_t srcMaxOffsetBytes = count * sizeof(PrimT<T>);
    uint64_t dstMaxOffsetBytes = count * sizeof(PrimT<T>);
    std::string apiInfo = "DataCopy from " + GetPositionDisplay(static_cast<TPosition>(src.GetPosition())) + " to " +
                          GetPositionDisplay(static_cast<TPosition>(dst.GetPosition()));

    return check::ReportTensorSizeOverflow(
        srcPos, dstPos, src.GetSize() * sizeof(PrimT<T>), dst.GetSize() * sizeof(PrimT<T>), srcMaxOffsetBytes,
        dstMaxOffsetBytes, apiInfo);
}

template <typename T>
bool CheckDataCopyTensorSizeOverflow(const GlobalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count)
{
    const Hardware srcPos = GetPhyType((TPosition)src.GetPosition());
    const Hardware dstPos = Hardware::GM;
    uint64_t srcMaxOffsetBytes = count * sizeof(PrimT<T>);
    uint64_t dstMaxOffsetBytes = 0;
    std::string apiInfo = "DataCopy from " + GetPositionDisplay(static_cast<TPosition>(src.GetPosition())) + " to " +
                          GetPositionDisplay(TPosition::GM);

    return check::ReportTensorSizeOverflow(
        srcPos, dstPos, src.GetSize() * sizeof(PrimT<T>), 0, srcMaxOffsetBytes, dstMaxOffsetBytes, apiInfo);
}

template <typename T>
bool CheckDataCopyTensorSizeOverflow(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const Nz2NdParamsFull& intriParams)
{
    if (intriParams.ndNum == 0 || intriParams.nValue == 0 || intriParams.dValue == 0) {
        return true;
    }
    const Hardware srcPos = GetPhyType((TPosition)src.GetPosition());
    const Hardware dstPos = Hardware::GM;
    const int nzWidth = 16;
    const int srcMatrixStrideUnit = 256;
    uint64_t srcMaxOffsetBytes =
        (intriParams.ndNum - 1) * intriParams.srcNdMatrixStride * srcMatrixStrideUnit * sizeof(PrimT<T>) +
        (intriParams.dValue / nzWidth - 1) * intriParams.srcNStride * nzWidth * sizeof(PrimT<T>) +
        nzWidth * intriParams.nValue * sizeof(PrimT<T>);
    uint64_t dstMaxOffsetBytes = 0;
    std::string apiInfo = "DataCopy with Nz2NdParamsFull from " +
                          GetPositionDisplay(static_cast<TPosition>(src.GetPosition())) + " to " +
                          GetPositionDisplay(TPosition::GM);

    return check::ReportTensorSizeOverflow(
        srcPos, dstPos, src.GetSize() * sizeof(PrimT<T>), 0, srcMaxOffsetBytes, dstMaxOffsetBytes, apiInfo);
}

template <typename T>
bool CheckDataCopyTensorSizeOverflow(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
    if (intriParams.blockCount == 0 || intriParams.blockLen == 0) {
        return true;
    }
    const Hardware srcPos = Hardware::GM;
    const Hardware dstPos = GetPhyType((TPosition)dst.GetPosition());
    uint64_t srcMaxOffsetBytes = 0;
    uint64_t dstMaxOffsetBytes = 0;
    BlockMode mode = check::GetBlockMode({srcPos, dstPos}, enhancedParams.blockMode);
    uint8_t biasConvFlag = 0;
    std::string apiInfo = "DataCopy with DataCopyEnhancedParams from " + GetPositionDisplay(TPosition::GM) + " to " +
                          GetPositionDisplay(static_cast<TPosition>(dst.GetPosition()));

    check::CalculateDataCopyMaxOffset<PrimT<T>, PrimT<T>>(
        intriParams, srcPos, dstPos, mode, srcMaxOffsetBytes, dstMaxOffsetBytes, enhancedParams.deqScale, biasConvFlag);
    return check::ReportTensorSizeOverflow(
        srcPos, dstPos, 0, dst.GetSize() * sizeof(PrimT<T>), srcMaxOffsetBytes, dstMaxOffsetBytes, apiInfo);
}

template <typename T>
bool CheckDataCopyTensorSizeOverflow(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
    if (intriParams.blockCount == 0 || intriParams.blockLen == 0) {
        return true;
    }
    const Hardware srcPos = GetPhyType((TPosition)src.GetPosition());
    const Hardware dstPos = Hardware::GM;
    uint64_t srcMaxOffsetBytes = 0;
    uint64_t dstMaxOffsetBytes = 0;
    BlockMode mode = check::GetBlockMode({srcPos, dstPos}, enhancedParams.blockMode);
    uint8_t biasConvFlag = 0;
    std::string apiInfo = "DataCopy with DataCopyEnhancedParams from " +
                          GetPositionDisplay(static_cast<TPosition>(src.GetPosition())) + " to " +
                          GetPositionDisplay(TPosition::GM);

    check::CalculateDataCopyMaxOffset<PrimT<T>, PrimT<T>>(
        intriParams, srcPos, dstPos, mode, srcMaxOffsetBytes, dstMaxOffsetBytes, enhancedParams.deqScale, biasConvFlag);
    return check::ReportTensorSizeOverflow(
        srcPos, dstPos, src.GetSize() * sizeof(PrimT<T>), 0, srcMaxOffsetBytes, dstMaxOffsetBytes, apiInfo);
}

template <typename T>
bool CheckDataCopyTensorSizeOverflow(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
    if (intriParams.blockCount == 0 || intriParams.blockLen == 0) {
        return true;
    }
    const Hardware srcPos = GetPhyType((TPosition)src.GetPosition());
    const Hardware dstPos = GetPhyType((TPosition)dst.GetPosition());
    uint64_t srcMaxOffsetBytes = 0;
    uint64_t dstMaxOffsetBytes = 0;
    BlockMode mode = check::GetBlockMode({srcPos, dstPos}, enhancedParams.blockMode);
    uint8_t biasConvFlag = 0;
    std::string apiInfo = "DataCopy with DataCopyEnhancedParams from " +
                          GetPositionDisplay(static_cast<TPosition>(src.GetPosition())) + " to " +
                          GetPositionDisplay(static_cast<TPosition>(dst.GetPosition()));

    check::CalculateDataCopyMaxOffset<PrimT<T>, PrimT<T>>(
        intriParams, srcPos, dstPos, mode, srcMaxOffsetBytes, dstMaxOffsetBytes, enhancedParams.deqScale, biasConvFlag);
    return check::ReportTensorSizeOverflow(
        srcPos, dstPos, src.GetSize() * sizeof(PrimT<T>), dst.GetSize() * sizeof(PrimT<T>), srcMaxOffsetBytes,
        dstMaxOffsetBytes, apiInfo);
}

template <typename T, typename U>
bool CheckDataCopyTensorSizeOverflow(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyCO12DstParams& intriParams)
{
    if (intriParams.nSize == 0) {
        return true;
    }
    const Hardware srcPos = GetPhyType((TPosition)src.GetPosition());
    const Hardware dstPos = GetPhyType((TPosition)dst.GetPosition());
    uint64_t srcMaxOffsetBytes = 0;
    uint64_t dstMaxOffsetBytes = 0;
    std::string apiInfo = "DataCopy with DataCopyCO12DstParams from " +
                          GetPositionDisplay(static_cast<TPosition>(src.GetPosition())) + " to " +
                          GetPositionDisplay(static_cast<TPosition>(dst.GetPosition()));

    check::CalculateDataCopyMaxOffset<PrimT<U>, PrimT<T>>(
        srcPos, dstPos, intriParams, srcMaxOffsetBytes, dstMaxOffsetBytes);
    return check::ReportTensorSizeOverflow(
        srcPos, dstPos, src.GetSize() * sizeof(PrimT<U>), dst.GetSize() * sizeof(PrimT<T>), srcMaxOffsetBytes,
        dstMaxOffsetBytes, apiInfo);
}

template <typename T, typename U>
bool CheckDataCopyTensorSizeOverflow(
    const GlobalTensor<T>& dst, const LocalTensor<U>& src, const DataCopyCO12DstParams& intriParams)
{
    if (intriParams.nSize == 0) {
        return true;
    }
    const Hardware srcPos = GetPhyType((TPosition)src.GetPosition());
    const Hardware dstPos = Hardware::GM;
    uint64_t srcMaxOffsetBytes = 0;
    uint64_t dstMaxOffsetBytes = 0;
    std::string apiInfo = "DataCopy with DataCopyCO12DstParams from " +
                          GetPositionDisplay(static_cast<TPosition>(src.GetPosition())) + " to " +
                          GetPositionDisplay(TPosition::GM);

    check::CalculateDataCopyMaxOffset<PrimT<U>, PrimT<T>>(
        srcPos, dstPos, intriParams, srcMaxOffsetBytes, dstMaxOffsetBytes);
    return check::ReportTensorSizeOverflow(
        srcPos, dstPos, src.GetSize() * sizeof(PrimT<U>), 0, srcMaxOffsetBytes, dstMaxOffsetBytes, apiInfo);
}

template <typename T, typename U>
bool CheckDataCopyTensorSizeOverflow(
    const LocalTensor<U>& dst, const LocalTensor<T>& src, const DataCopyParams& intriParams,
    const DataCopyEnhancedParams& enhancedParams)
{
    if (intriParams.blockCount == 0 || intriParams.blockLen == 0) {
        return true;
    }
    const Hardware srcPos = GetPhyType((TPosition)src.GetPosition());
    const Hardware dstPos = GetPhyType((TPosition)dst.GetPosition());
    uint64_t srcMaxOffsetBytes = 0;
    uint64_t dstMaxOffsetBytes = 0;
    BlockMode mode = check::GetBlockMode({srcPos, dstPos}, enhancedParams.blockMode);
    uint8_t biasConvFlag = check::IsBiasConv({srcPos, dstPos}) && (sizeof(PrimT<T>) != sizeof(PrimT<U>));
    std::string apiInfo = "DataCopy with DataCopyEnhancedParams from " +
                          GetPositionDisplay(static_cast<TPosition>(src.GetPosition())) + " to " +
                          GetPositionDisplay(static_cast<TPosition>(dst.GetPosition()));

    check::CalculateDataCopyMaxOffset<PrimT<T>, PrimT<U>>(
        intriParams, srcPos, dstPos, mode, srcMaxOffsetBytes, dstMaxOffsetBytes, enhancedParams.deqScale, biasConvFlag,
        enhancedParams.sidStoreMode);
    return check::ReportTensorSizeOverflow(
        srcPos, dstPos, src.GetSize() * sizeof(PrimT<T>), dst.GetSize() * sizeof(PrimT<U>), srcMaxOffsetBytes,
        dstMaxOffsetBytes, apiInfo);
}

template <typename T>
bool CheckDataCopyPadTensorSizeOverflow(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyParams& dataCopyParams,
    const DataCopyPadParams& padParams)
{
    const Hardware srcPos = Hardware::GM;
    const Hardware dstPos = GetPhyType((TPosition)dst.GetPosition());
    if (dstPos != Hardware::UB) {
        return true;
    }
    uint64_t srcMaxOffsetBytes = 0;
    uint64_t paddingSize = (padParams.leftPadding + padParams.rightPadding) * sizeof(PrimT<T>);
    uint64_t dstMaxOffsetBytes =
        dataCopyParams.blockCount * AlignUp(dataCopyParams.blockLen + paddingSize, DEFAULT_C0_SIZE) +
        (dataCopyParams.blockCount - 1) * dataCopyParams.dstStride * DEFAULT_C0_SIZE;
    std::string apiInfo = "DataCopyPad with DataCopyParams and DataCopyPadParams from " +
                          GetPositionDisplay(TPosition::GM) + " to " +
                          GetPositionDisplay(static_cast<TPosition>(dst.GetPosition()));

    return check::ReportTensorSizeOverflow(
        srcPos, dstPos, 0, dst.GetSize() * sizeof(PrimT<T>), srcMaxOffsetBytes, dstMaxOffsetBytes, apiInfo);
}

template <typename T>
bool CheckDataCopyPadTensorSizeOverflow(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& dataCopyParams)
{
    const Hardware srcPos = GetPhyType((TPosition)src.GetPosition());
    const Hardware dstPos = Hardware::GM;
    if (srcPos != Hardware::UB) {
        return true;
    }
    uint64_t srcMaxOffsetBytes = dataCopyParams.blockCount * AlignUp(dataCopyParams.blockLen, DEFAULT_C0_SIZE) +
                                 (dataCopyParams.blockCount - 1) * dataCopyParams.srcStride * DEFAULT_C0_SIZE;
    uint64_t dstMaxOffsetBytes = 0;
    std::string apiInfo = "DataCopyPad with DataCopyParams from " +
                          GetPositionDisplay(static_cast<TPosition>(src.GetPosition())) + " to " +
                          GetPositionDisplay(TPosition::GM);

    return check::ReportTensorSizeOverflow(
        srcPos, dstPos, src.GetSize() * sizeof(PrimT<T>), 0, srcMaxOffsetBytes, dstMaxOffsetBytes, apiInfo);
}

template <typename T, typename U>
bool CheckDataCopyPadTensorSizeOverflow(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyExtParams& dataCopyParams,
    const DataCopyPadExtParams<U>& padParams)
{
    const Hardware srcPos = Hardware::GM;
    const Hardware dstPos = GetPhyType((TPosition)dst.GetPosition());
    if (dstPos != Hardware::UB) {
        return true;
    }
    uint64_t srcMaxOffsetBytes = 0;
    uint64_t paddingSize = (padParams.leftPadding + padParams.rightPadding) * sizeof(PrimT<T>);
    uint64_t dstMaxOffsetBytes =
        dataCopyParams.blockCount * AlignUp(dataCopyParams.blockLen + paddingSize, DEFAULT_C0_SIZE) +
        (dataCopyParams.blockCount - 1) * dataCopyParams.dstStride * DEFAULT_C0_SIZE;
    std::string apiInfo = "DataCopyPad with DataCopyExtParams and DataCopyPadExtParams from " +
                          GetPositionDisplay(TPosition::GM) + " to " +
                          GetPositionDisplay(static_cast<TPosition>(dst.GetPosition()));

    return check::ReportTensorSizeOverflow(
        srcPos, dstPos, 0, dst.GetSize() * sizeof(PrimT<T>), srcMaxOffsetBytes, dstMaxOffsetBytes, apiInfo);
}

template <typename T>
bool CheckDataCopyPadTensorSizeOverflow(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams& dataCopyParams)
{
    const Hardware srcPos = GetPhyType((TPosition)src.GetPosition());
    const Hardware dstPos = Hardware::GM;
    if (srcPos != Hardware::UB) {
        return true;
    }
    uint64_t srcMaxOffsetBytes = dataCopyParams.blockCount * AlignUp(dataCopyParams.blockLen, DEFAULT_C0_SIZE) +
                                 (dataCopyParams.blockCount - 1) * dataCopyParams.srcStride * DEFAULT_C0_SIZE;
    uint64_t dstMaxOffsetBytes = 0;
    std::string apiInfo = "DataCopyPad with DataCopyExtParams from " +
                          GetPositionDisplay(static_cast<TPosition>(src.GetPosition())) + " to " +
                          GetPositionDisplay(TPosition::GM);

    return check::ReportTensorSizeOverflow(
        srcPos, dstPos, src.GetSize() * sizeof(PrimT<T>), 0, srcMaxOffsetBytes, dstMaxOffsetBytes, apiInfo);
}

template <typename T>
bool CheckDataCopyPadTensorSizeOverflow(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& dataCopyParams,
    const Nd2NzParams& nd2nzParams)
{
    if (dataCopyParams.blockCount == 0 || nd2nzParams.nValue == 0 || nd2nzParams.dValue == 0 ||
        nd2nzParams.ndNum == 0) {
        return true;
    }
    const Hardware srcPos = GetPhyType((TPosition)src.GetPosition());
    const Hardware dstPos = GetPhyType((TPosition)dst.GetPosition());
    uint64_t srcMaxOffsetBytes = dataCopyParams.blockCount * AlignUp(dataCopyParams.blockLen, DEFAULT_C0_SIZE) +
                                 (dataCopyParams.blockCount - 1) * dataCopyParams.srcStride * DEFAULT_C0_SIZE;
    uint64_t dstMaxOffsetBytes = (nd2nzParams.ndNum - 1) * nd2nzParams.dstNzMatrixStride * sizeof(PrimT<T>) +
                                 (nd2nzParams.nValue - 1) * nd2nzParams.dstNzNStride * DEFAULT_C0_SIZE +
                                 (DivCeil(nd2nzParams.dValue * sizeof(PrimT<T>), DEFAULT_C0_SIZE) - 1) *
                                     nd2nzParams.dstNzC0Stride * DEFAULT_C0_SIZE +
                                 DEFAULT_C0_SIZE;
    std::string apiInfo = "DataCopyPad with DataCopyParams and Nd2NzParams from " +
                          GetPositionDisplay(static_cast<TPosition>(src.GetPosition())) + " to " +
                          GetPositionDisplay(static_cast<TPosition>(dst.GetPosition()));

    return check::ReportTensorSizeOverflow(
        srcPos, dstPos, src.GetSize() * sizeof(PrimT<T>), dst.GetSize() * sizeof(PrimT<T>), srcMaxOffsetBytes,
        dstMaxOffsetBytes, apiInfo);
}

template <typename T>
bool CheckDataCopyPadTensorSizeOverflow(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyExtParams& dataCopyParams,
    const Nd2NzParams& nd2nzParams)
{
    if (dataCopyParams.blockCount == 0 || nd2nzParams.nValue == 0 || nd2nzParams.dValue == 0 ||
        nd2nzParams.ndNum == 0) {
        return true;
    }
    const Hardware srcPos = GetPhyType((TPosition)src.GetPosition());
    const Hardware dstPos = GetPhyType((TPosition)dst.GetPosition());
    uint64_t srcMaxOffsetBytes = dataCopyParams.blockCount * AlignUp(dataCopyParams.blockLen, DEFAULT_C0_SIZE) +
                                 (dataCopyParams.blockCount - 1) * dataCopyParams.srcStride * DEFAULT_C0_SIZE;
    uint64_t dstMaxOffsetBytes = (nd2nzParams.ndNum - 1) * nd2nzParams.dstNzMatrixStride * sizeof(PrimT<T>) +
                                 (nd2nzParams.nValue - 1) * nd2nzParams.dstNzNStride * DEFAULT_C0_SIZE +
                                 (DivCeil(nd2nzParams.dValue * sizeof(PrimT<T>), DEFAULT_C0_SIZE) - 1) *
                                     nd2nzParams.dstNzC0Stride * DEFAULT_C0_SIZE +
                                 DEFAULT_C0_SIZE;
    std::string apiInfo = "DataCopyPad with DataCopyExtParams and Nd2NzParams from " +
                          GetPositionDisplay(static_cast<TPosition>(src.GetPosition())) + " to " +
                          GetPositionDisplay(static_cast<TPosition>(dst.GetPosition()));

    return check::ReportTensorSizeOverflow(
        srcPos, dstPos, src.GetSize() * sizeof(PrimT<T>), dst.GetSize() * sizeof(PrimT<T>), srcMaxOffsetBytes,
        dstMaxOffsetBytes, apiInfo);
}

} // namespace AscendC
#endif

#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_CHECK_DATA_COPY_OVERFLOW_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_CHECK_DATA_COPY_OVERFLOW_H__
#endif
