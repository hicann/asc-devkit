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
 * \file kernel_check_copy.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/utils/kernel_check_copy.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_CHECK_COPY_H__
#endif
#ifndef ASCENDC_MODULE_CHECK_COPY_H
#define ASCENDC_MODULE_CHECK_COPY_H

#if ASCENDC_CPU_DEBUG
#include "../kernel_check_util.h"
#include "../../../include/basic_api/kernel_common.h"
#include "../../../include/basic_api/kernel_struct_data_copy.h"

namespace AscendC {
/* **************************************************************************************************
 * Check function for CPU debug
 * ************************************************************************************************* */
template <typename T>
check::CopyApiParams BuildCopyApiParams(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint8_t repeatTime,
    const CopyRepeatParams& repeatParams)
{
    return check::CopyApiParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        repeatTime,
        static_cast<uint16_t>(repeatParams.dstStride),
        static_cast<uint16_t>(repeatParams.srcStride),
        static_cast<uint16_t>(repeatParams.dstRepeatSize),
        static_cast<uint16_t>(repeatParams.srcRepeatSize),
        static_cast<uint32_t>(sizeof(T)),
        static_cast<uint32_t>(sizeof(T)),
        static_cast<uint64_t>(dst.GetSize() * sizeof(T)),
        static_cast<uint64_t>(src.GetSize() * sizeof(T)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src.GetPosition())};
}

template <typename T>
bool CheckFuncCopy(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint64_t mask, const uint8_t repeatTime,
    const CopyRepeatParams& repeatParams, const char* intriName)
{
    auto chkParams = BuildCopyApiParams(dst, src, repeatTime, repeatParams);
    return CheckFuncCopyImpl(chkParams, mask, intriName);
}

template <typename T>
bool CheckFuncCopy(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint64_t mask[], const uint8_t repeatTime,
    const CopyRepeatParams& repeatParams, const char* intriName)
{
    auto chkParams = BuildCopyApiParams(dst, src, repeatTime, repeatParams);
    return CheckFuncCopyImplForMaskArray(chkParams, mask, intriName);
}

template <typename T>
bool CheckFuncCopy(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count, const char* intriName)
{
    check::CopyApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        static_cast<uint32_t>(sizeof(T)),
        static_cast<uint32_t>(sizeof(T)),
        static_cast<uint64_t>(dst.GetSize() * sizeof(T)),
        static_cast<uint64_t>(src.GetSize() * sizeof(T)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src.GetPosition()),
        static_cast<uint32_t>(count)};
    return CheckFuncCopyImpl(chkParams, intriName);
}

template <typename T>
bool CheckFuncDataCopy(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams, const char* intriName)
{
    check::DataCopyApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        static_cast<uint32_t>(sizeof(T)),
        static_cast<uint32_t>(sizeof(T)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src.GetPosition()),
        repeatParams.blockCount,
        repeatParams.blockLen,
        repeatParams.srcStride,
        repeatParams.dstStride};
    return CheckFuncDataCopyImpl(chkParams, intriName);
}

template <typename T>
bool CheckFuncDataCopy(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyParams& repeatParams, const char* intriName)
{
    check::DataCopyApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        static_cast<uint32_t>(sizeof(T)),
        static_cast<uint32_t>(sizeof(T)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(Hardware::GM),
        repeatParams.blockCount,
        repeatParams.blockLen,
        repeatParams.srcStride,
        repeatParams.dstStride};
    return CheckFuncDataCopyImpl(chkParams, intriName);
}

template <typename T>
bool CheckFuncDataCopyPad(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyParams& dataCopyParams,
    const DataCopyPadParams& padParams, const char* intriName)
{
    check::DataCopyPadApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        static_cast<uint32_t>(sizeof(T)),
        static_cast<uint32_t>(sizeof(T)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(Hardware::GM),
        dataCopyParams.blockCount,
        dataCopyParams.blockLen,
        dataCopyParams.srcStride,
        dataCopyParams.dstStride,
        padParams.isPad,
        padParams.leftPadding,
        padParams.rightPadding,
        padParams.paddingValue};
    return CheckFuncDataCopyPadImpl(chkParams, intriName);
}

template <typename T>
bool CheckFuncDataCopyPad(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const DataCopyExtParams& dataCopyParams,
    const DataCopyPadExtParams<T>& padParams, const char* intriName)
{
    check::DataCopyPadApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        static_cast<uint32_t>(sizeof(T)),
        static_cast<uint32_t>(sizeof(T)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(Hardware::GM),
        dataCopyParams.blockCount,
        static_cast<uint16_t>(dataCopyParams.blockLen),
        static_cast<uint64_t>(dataCopyParams.srcStride),
        static_cast<uint64_t>(dataCopyParams.dstStride),
        padParams.isPad,
        padParams.leftPadding,
        padParams.rightPadding,
        GetScalarBitcodeValue(padParams.paddingValue)};
    return CheckFuncDataCopyPadImpl(chkParams, intriName);
}

template <typename T>
bool CheckFuncDataCopy(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const DataCopyParams& repeatParams, const char* intriName)
{
    check::DataCopyApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        static_cast<uint32_t>(sizeof(T)),
        static_cast<uint32_t>(sizeof(T)),
        static_cast<uint8_t>(Hardware::GM),
        static_cast<uint8_t>(src.GetPosition()),
        repeatParams.blockCount,
        repeatParams.blockLen,
        repeatParams.srcStride,
        repeatParams.dstStride};
    return CheckFuncDataCopyImpl(chkParams, intriName);
}

template <typename T>
bool CheckFuncDataCopySlice(
    const LocalTensor<T>& dst, const GlobalTensor<T>& src, const SliceInfo dstSliceInfo[],
    const SliceInfo srcSliceInfo[], const uint32_t dimValue, const char* intriName)
{
    bool isGM2UB = true;
    uint32_t srcShapeInfo[K_MAX_SHAPE_DIM];
    uint32_t dstShapeInfo[K_MAX_SHAPE_DIM];
    bool useShapeValue = !(srcSliceInfo[0].shapeValue == 0);
    for (int i = 0; i < dimValue; i++) {
        srcShapeInfo[i] = useShapeValue ? srcSliceInfo[i].shapeValue : src.GetShapeInfo().shape[i];
        dstShapeInfo[i] = useShapeValue ? dstSliceInfo[i].shapeValue : dst.GetShapeInfo().shape[i];
    }

    check::DataCopySliceApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        static_cast<uint32_t>(sizeof(T)),
        static_cast<uint32_t>(sizeof(T)),
        static_cast<uint64_t>(dst.GetSize() * sizeof(T)),
        static_cast<uint8_t>(dst.GetPosition()),
        dimValue,
        dstShapeInfo,
        srcShapeInfo,
        dstSliceInfo,
        srcSliceInfo,
        isGM2UB};
    return CheckFuncDataCopySliceImpl(chkParams, intriName);
}

template <typename T>
bool CheckFuncDataCopySlice(
    const GlobalTensor<T>& dst, const LocalTensor<T>& src, const SliceInfo dstSliceInfo[],
    const SliceInfo srcSliceInfo[], const uint32_t dimValue, const char* intriName)
{
    bool isGM2UB = false;
    uint32_t srcShapeInfo[K_MAX_SHAPE_DIM];
    uint32_t dstShapeInfo[K_MAX_SHAPE_DIM];
    bool useShapeValue = !(srcSliceInfo[0].shapeValue == 0);
    for (int i = 0; i < dimValue; i++) {
        srcShapeInfo[i] = useShapeValue ? srcSliceInfo[i].shapeValue : src.GetShapeInfo().shape[i];
        dstShapeInfo[i] = useShapeValue ? dstSliceInfo[i].shapeValue : dst.GetShapeInfo().shape[i];
    }

    check::DataCopySliceApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        static_cast<uint32_t>(sizeof(T)),
        static_cast<uint32_t>(sizeof(T)),
        static_cast<uint64_t>(src.GetSize() * sizeof(T)),
        static_cast<uint8_t>(src.GetPosition()),
        dimValue,
        dstShapeInfo,
        srcShapeInfo,
        dstSliceInfo,
        srcSliceInfo,
        isGM2UB};
    return CheckFuncDataCopySliceImpl(chkParams, intriName);
}
} // namespace AscendC
#endif

#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_CHECK_COPY_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_CHECK_COPY_H__
#endif
