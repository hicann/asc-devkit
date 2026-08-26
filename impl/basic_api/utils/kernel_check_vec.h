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
 * \file kernel_check_vec.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/utils/kernel_check_vec.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_CHECK_VEC_H__
#endif
#ifndef ASCENDC_MODULE_CHECK_VEC_H
#define ASCENDC_MODULE_CHECK_VEC_H

#if ASCENDC_CPU_DEBUG
#include "../kernel_check_util.h"
#include "../../../include/basic_api/kernel_common.h"
#include "../../../include/basic_api/kernel_struct_unary.h"
#include "../../../include/basic_api/kernel_struct_mm.h"

namespace AscendC {
template <typename T, typename U>
bool CheckVectorPadding(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint8_t padMode, const bool padSide,
    const uint64_t mask[], const uint8_t repeatTime, const UnaryRepeatParams& repeatParams, const char* intriName)
{
    check::VectorPaddingApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        repeatTime,
        static_cast<uint16_t>(repeatParams.dstBlkStride),
        static_cast<uint16_t>(repeatParams.srcBlkStride),
        static_cast<uint16_t>(repeatParams.dstRepStride),
        static_cast<uint16_t>(repeatParams.srcRepStride),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(sizeof(PrimT<U>)),
        static_cast<uint64_t>(dst.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint64_t>(src.GetSize() * sizeof(PrimT<U>)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src.GetPosition()),
        padMode,
        padSide};
    return CheckVectorPaddingForMaskArray(chkParams, mask, intriName);
}

template <typename T, typename U>
bool CheckVectorPadding(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint8_t padMode, const bool padSide,
    const uint64_t mask, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams, const char* intriName)
{
    check::VectorPaddingApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        repeatTime,
        static_cast<uint16_t>(repeatParams.dstBlkStride),
        static_cast<uint16_t>(repeatParams.srcBlkStride),
        static_cast<uint16_t>(repeatParams.dstRepStride),
        static_cast<uint16_t>(repeatParams.srcRepStride),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(sizeof(PrimT<U>)),
        static_cast<uint64_t>(dst.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint64_t>(src.GetSize() * sizeof(PrimT<U>)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src.GetPosition()),
        padMode,
        padSide};
    return CheckVectorPadding(chkParams, mask, intriName);
}

template <typename T, typename U>
bool CheckVectorPadding(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const uint8_t padMode, const bool padSide,
    const uint32_t count, const char* intriName)
{
    check::VectorPaddingApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(sizeof(PrimT<U>)),
        static_cast<uint64_t>(dst.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint64_t>(src.GetSize() * sizeof(PrimT<U>)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src.GetPosition()),
        count,
        padMode,
        padSide};
    return CheckVectorPadding(chkParams, intriName);
}

template <typename T>
bool CheckFuncLoadDataTranspose(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2dTransposeParams& loadDataParams,
    const char* intriName)
{
#if __NPU_ARCH__ == 3102
    constexpr bool dtypeMatch = SupportType<PrimT<T>, uint8_t, int8_t, half>();
    ASSERT(dtypeMatch && "LoadData2dTransposeParams without dtype of u8/s8/fp16 is not supported on current device");
    return dtypeMatch;
#elif (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
    constexpr bool dtypeMatch = SupportType<PrimT<T>, uint8_t, int8_t, half, bfloat16_t, float, int32_t, uint32_t>();
    ASSERT(
        dtypeMatch &&
        "LoadData2dTransposeParams without dtype of u8/s8/fp16/bf16/f32/s32/u32 is not supported on current device");
    return dtypeMatch;
#else
    return true;
#endif
}

template <typename T>
bool CheckFuncLoadDataTranspose(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LoadData2dTransposeParamsV2& loadDataParams,
    const char* intriName)
{
#if __NPU_ARCH__ == 3102 || (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
    bool scopeMatch =
        (GetPhyType(static_cast<TPosition>(dst.GetPosition())) == Hardware::L0B &&
         GetPhyType(static_cast<TPosition>(src.GetPosition())) == Hardware::L1);
    ASSERT(
        scopeMatch && "LoadDataWithTranspose without L1 Buffer(B1)->L0B Buffer(B2) is not supported on current device");
#if __NPU_ARCH__ == 3102
    constexpr bool dtypeMatch =
        IsSameType<PrimT<T>, int4b_t>::value || sizeof(PrimT<T>) == sizeof(int8_t) || sizeof(PrimT<T>) == sizeof(half);
#elif (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
    constexpr bool dtypeMatch =
        sizeof(PrimT<T>) == sizeof(int8_t) || sizeof(PrimT<T>) == sizeof(half) || sizeof(PrimT<T>) == sizeof(float);
#endif
    ASSERT(dtypeMatch && "LoadDataWithTranspose is not supported on current device");
    return scopeMatch && dtypeMatch;
#else
    ASSERT(false && "Current version don't support LoadDataWithTranspose using LoadData2dTransposeParamsV2");
    return false;
#endif
}

template <typename T, typename U, typename S, typename V>
bool CheckMmadParams(
    const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const LocalTensor<V>& bias,
    const MmadParams& mmadParams, const char* intriName)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    return true;
#else
    check::MmadApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(fm.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(filter.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(bias.GetPhyAddr())),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(sizeof(PrimT<U>)),
        static_cast<uint32_t>(sizeof(PrimT<S>)),
        static_cast<uint32_t>(sizeof(PrimT<V>)),
        static_cast<uint64_t>(dst.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint64_t>(fm.GetSize() * sizeof(PrimT<U>)),
        static_cast<uint64_t>(filter.GetSize() * sizeof(PrimT<S>)),
        static_cast<uint64_t>(bias.GetSize() * sizeof(PrimT<V>)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(fm.GetPosition()),
        static_cast<uint8_t>(filter.GetPosition()),
        static_cast<uint8_t>(bias.GetPosition()),
        mmadParams.m,
        mmadParams.n,
        mmadParams.k,
        mmadParams.isBias,
        mmadParams.fmOffset,
        mmadParams.enSsparse,
        mmadParams.enWinogradA,
        mmadParams.enWinogradB};
    return CheckFuncMmadImpl(chkParams, intriName);
#endif
}
template <typename T, typename U, typename S>
bool CheckMmadParams(
    const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const MmadParams& mmadParams,
    const char* intriName)
{
#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3003 || __NPU_ARCH__ == 3113 || __NPU_ARCH__ == 5101 || \
                              __NPU_ARCH__ == 5161 || __NPU_ARCH__ == 5165 || __NPU_ARCH__ == 5163)
    return true;
#else
    check::MmadApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(fm.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(filter.GetPhyAddr())),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(sizeof(PrimT<U>)),
        static_cast<uint32_t>(sizeof(PrimT<S>)),
        static_cast<uint64_t>(dst.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint64_t>(fm.GetSize() * sizeof(PrimT<U>)),
        static_cast<uint64_t>(filter.GetSize() * sizeof(PrimT<S>)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(fm.GetPosition()),
        static_cast<uint8_t>(filter.GetPosition()),
        mmadParams.m,
        mmadParams.n,
        mmadParams.k,
        mmadParams.isBias,
        mmadParams.fmOffset,
        mmadParams.enSsparse,
        mmadParams.enWinogradA,
        mmadParams.enWinogradB};
    return CheckFuncMmadImpl(chkParams, intriName);
#endif
}

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
template <typename T, typename U, typename S, typename V>
bool CheckMmadParams(
    const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const LocalTensor<V>& bias,
    const uint64_t& mmadParams, const char* intriName)
{
    return true;
}

template <typename T, typename U, typename S>
bool CheckMmadParams(
    const LocalTensor<T>& dst, const LocalTensor<U>& fm, const LocalTensor<S>& filter, const uint64_t& mmadParams,
    const char* intriName)
{
    return true;
}
#endif

template <typename T, typename U>
bool CheckFuncBroadCastToMM(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const int32_t blockCount, const uint8_t blockLen,
    const uint8_t srcGap, const uint8_t dstGap, const char* intriName)
{
    check::VecBroadCastToMMApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(sizeof(PrimT<U>)),
        static_cast<uint64_t>(dst.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint64_t>(src.GetSize() * sizeof(PrimT<U>)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src.GetPosition()),
        static_cast<uint32_t>(blockCount),
        static_cast<uint8_t>(blockLen),
        static_cast<uint8_t>(srcGap),
        static_cast<uint8_t>(dstGap)};
    return CheckFuncBroadCastToMMImpl(chkParams, intriName);
}

template <typename T, typename U = T>
check::VecReduceApiParams BuildVecReduceOtherParams(
    const LocalTensor<U>& dst, const LocalTensor<T>& src, const int32_t repeatTime, const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride)
{
    return check::VecReduceApiParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        static_cast<uint32_t>(sizeof(PrimT<U>)),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        repeatTime,
        static_cast<uint16_t>(dstRepStride),
        static_cast<uint16_t>(srcBlkStride),
        static_cast<uint16_t>(srcRepStride),
        static_cast<uint64_t>(dst.GetSize() * sizeof(PrimT<U>)),
        static_cast<uint64_t>(src.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src.GetPosition())};
}

template <typename T, typename U = T>
bool CheckFunVecReduceOther(
    const LocalTensor<U>& dst, const LocalTensor<T>& src, const int32_t repeatTime, const int32_t maskCount,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, const char* intriName)
{
    auto chkParams = BuildVecReduceOtherParams(dst, src, repeatTime, dstRepStride, srcBlkStride, srcRepStride);
    return CheckFunReduceOtherImpl(chkParams, maskCount, intriName);
}

template <typename T, typename U = T>
bool CheckFunVecReduceOther(
    const LocalTensor<U>& dst, const LocalTensor<T>& src, const int32_t repeatTime, const uint64_t mask[],
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, const char* intriName)
{
    auto chkParams = BuildVecReduceOtherParams(dst, src, repeatTime, dstRepStride, srcBlkStride, srcRepStride);
    return CheckFunReduceOtherImplForMaskArray(chkParams, mask, intriName);
}

template <typename T>
check::VecReduceWhlApiParams BuildVecReduceOtherWhlParams(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t repeatTime, const int32_t dstRepStride,
    const int32_t srcBlkStride, const int32_t srcRepStride, ReduceOrder order)
{
    return check::VecReduceWhlApiParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        repeatTime,
        static_cast<uint16_t>(dstRepStride),
        static_cast<uint16_t>(srcBlkStride),
        static_cast<uint16_t>(srcRepStride),
        order,
        static_cast<uint64_t>(dst.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint64_t>(src.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src.GetPosition())};
}

template <typename T>
bool CheckFunVecReduceOtherWhl(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t repeatTime, const int32_t maskCount,
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, ReduceOrder order,
    const char* intriName)
{
    auto chkParams =
        BuildVecReduceOtherWhlParams(dst, src, repeatTime, dstRepStride, srcBlkStride, srcRepStride, order);
    return CheckFunReduceOtherWhlImpl(chkParams, maskCount, intriName);
}

template <typename T>
bool CheckFunVecReduceOtherWhl(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t repeatTime, const uint64_t mask[],
    const int32_t dstRepStride, const int32_t srcBlkStride, const int32_t srcRepStride, ReduceOrder order,
    const char* intriName)
{
    auto chkParams =
        BuildVecReduceOtherWhlParams(dst, src, repeatTime, dstRepStride, srcBlkStride, srcRepStride, order);
    return CheckFunReduceOtherWhlImplForMaskArray(chkParams, mask, intriName);
}

template <typename T>
check::VecReduceApiParams BuildVecReduceWithCalIndexParams(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& work, const int32_t repeatTime,
    bool calIndex, const int32_t srcRepStride)
{
    return check::VecReduceApiParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(work.GetPhyAddr())),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        repeatTime,
        calIndex,
        static_cast<uint64_t>(dst.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint64_t>(src.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint64_t>(work.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src.GetPosition()),
        static_cast<uint8_t>(work.GetPosition()),
        static_cast<uint16_t>(srcRepStride)};
}

template <typename T>
bool CheckFunVecReduce(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& work, const int32_t repeatTime,
    const int32_t mask, bool calIndex, const int32_t srcRepStride, const char* intriName)
{
    auto chkParams = BuildVecReduceWithCalIndexParams(dst, src, work, repeatTime, calIndex, srcRepStride);
    return CheckFunReduceImpl(chkParams, mask, intriName);
}

template <typename T>
bool CheckFunVecReduce(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& work, const int32_t repeatTime,
    const uint64_t mask[], bool calIndex, const int32_t srcRepStride, const char* intriName)
{
    auto chkParams = BuildVecReduceWithCalIndexParams(dst, src, work, repeatTime, calIndex, srcRepStride);
    return CheckFunReduceImplForMaskArray(chkParams, mask, intriName);
}

template <typename T>
check::VecReduceApiParams BuildVecReduceWithoutCalIndexParams(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& work, const int32_t repeatTime,
    const int32_t srcRepStride)
{
    return check::VecReduceApiParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(work.GetPhyAddr())),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        repeatTime,
        static_cast<uint64_t>(dst.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint64_t>(src.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint64_t>(work.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src.GetPosition()),
        static_cast<uint8_t>(work.GetPosition()),
        static_cast<uint16_t>(srcRepStride)};
}

template <typename T>
bool CheckFunVecReduce(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& work, const int32_t repeatTime,
    const int32_t mask, const int32_t srcRepStride, const char* intriName)
{
    auto chkParams = BuildVecReduceWithoutCalIndexParams(dst, src, work, repeatTime, srcRepStride);
    return CheckFunReduceImpl(chkParams, mask, intriName);
}

template <typename T>
bool CheckFunVecReduce(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& work, const int32_t repeatTime,
    const uint64_t mask[], const int32_t srcRepStride, const char* intriName)
{
    auto chkParams = BuildVecReduceWithoutCalIndexParams(dst, src, work, repeatTime, srcRepStride);
    return CheckFunReduceImplForMaskArray(chkParams, mask, intriName);
}

template <typename T>
bool CheckFunVecReduce(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& work, int32_t repeatTime,
    const int32_t count, bool calIndex, const char* intriName)
{
    // max or min level2
    check::VecReduceApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(work.GetPhyAddr())),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        repeatTime,
        static_cast<uint32_t>(count),
        calIndex,
        static_cast<uint64_t>(dst.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint64_t>(src.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint64_t>(work.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src.GetPosition()),
        static_cast<uint8_t>(work.GetPosition())};
    return CheckFunReduceImpl(chkParams, intriName);
}

template <typename T>
bool CheckFunVecReduce(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<T>& work, const int32_t count,
    int32_t repeatTime, const char* intriName)
{
    // sum level 2
    check::VecReduceApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(work.GetPhyAddr())),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        repeatTime,
        static_cast<uint32_t>(count),
        static_cast<uint64_t>(dst.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint64_t>(src.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint64_t>(work.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src.GetPosition()),
        static_cast<uint8_t>(work.GetPosition())};
    return CheckFunReduceImpl(chkParams, intriName);
}

template <typename T>
bool CheckFunVecReduceMode2(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t count, const char* intriName)
{
    check::VecReduceApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(count),
        static_cast<uint64_t>(dst.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint64_t>(src.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src.GetPosition())};
    return CheckFunReduceImplMode2(chkParams, intriName);
}

template <typename T, typename U>
check::VecScatterApiParams BuildVecScatterParams(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<U>& dstOffset, const uint32_t dstBaseAddr,
    const uint8_t repeatTime, const uint16_t srcRepStride)
{
    return check::VecScatterApiParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dstOffset.GetPhyAddr())),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(sizeof(PrimT<U>)),
        dstBaseAddr,
        repeatTime,
        static_cast<uint16_t>(srcRepStride),
        static_cast<uint64_t>(dst.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint64_t>(src.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint64_t>(dstOffset.GetSize() * sizeof(PrimT<U>)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src.GetPosition()),
        static_cast<uint8_t>(dstOffset.GetPosition())};
}

template <typename T, typename U>
bool CheckFunScatter(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<U>& dstOffset, const uint32_t dstBaseAddr,
    const uint64_t mask[], const uint8_t repeatTime, const uint16_t srcRepStride, const char* intriName)
{
    auto chkParams = BuildVecScatterParams(dst, src, dstOffset, dstBaseAddr, repeatTime, srcRepStride);
    return CheckFunScatterImplForMaskArray(chkParams, mask, intriName);
}

template <typename T, typename U>
bool CheckFunScatter(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<U>& dstOffset, const uint32_t dstBaseAddr,
    const uint64_t mask, const uint8_t repeatTime, const uint16_t srcRepStride, const char* intriName)
{
    auto chkParams = BuildVecScatterParams(dst, src, dstOffset, dstBaseAddr, repeatTime, srcRepStride);
    return CheckFunScatterImpl(chkParams, mask, intriName);
}

template <typename T, typename U>
bool CheckFunScatter(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const LocalTensor<U>& dstOffset, const uint32_t dstBaseAddr,
    const uint32_t count, const char* intriName)
{
    check::VecScatterApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dstOffset.GetPhyAddr())),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(sizeof(PrimT<U>)),
        dstBaseAddr,
        static_cast<uint32_t>(count),
        static_cast<uint64_t>(dst.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint64_t>(src.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint64_t>(dstOffset.GetSize() * sizeof(PrimT<U>)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src.GetPosition()),
        static_cast<uint8_t>(dstOffset.GetPosition())};
    return CheckFunScatterImpl(chkParams, intriName);
}
} // namespace AscendC
#endif

#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_CHECK_VEC_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_CHECK_VEC_H__
#endif
