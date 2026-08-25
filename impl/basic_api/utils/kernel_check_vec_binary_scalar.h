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
 * \file kernel_check_vec_binary_scalar.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/utils/kernel_check_vec_binary_scalar.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_tensor.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_CHECK_VEC_BINARY_SCALAR_H__
#endif
#ifndef ASCENDC_MODULE_CHECK_VEC_BINARY_SCALAR_H
#define ASCENDC_MODULE_CHECK_VEC_BINARY_SCALAR_H

#if ASCENDC_CPU_DEBUG
#include "../kernel_check_util.h"
#include "../../../include/basic_api/kernel_common.h"
#include "../../../include/basic_api/kernel_struct_binary.h"
#include "../../../include/basic_api/kernel_struct_unary.h"

namespace AscendC {
template <typename T, typename U>
bool CheckFuncVecBinaryScalarCmp(
    const LocalTensor<U>& dst, const LocalTensor<T>& src, const T& scalarValue, const uint64_t mask,
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams, const char* intriName)
{
    (void)(scalarValue);
    check::VecBinaryScalarApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        repeatTime,
        static_cast<uint16_t>(repeatParams.dstBlkStride),
        static_cast<uint16_t>(repeatParams.srcBlkStride),
        static_cast<uint16_t>(repeatParams.dstRepStride),
        static_cast<uint16_t>(repeatParams.srcRepStride),
        static_cast<uint32_t>(sizeof(PrimT<U>)),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint64_t>(dst.GetSize() * sizeof(PrimT<U>)),
        static_cast<uint64_t>(src.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src.GetPosition())};
    return CheckFuncVecBinaryScalarCmpImpl(chkParams, mask, intriName);
}

template <typename T, typename U>
bool CheckFuncVecBinaryScalarCmp(
    const LocalTensor<U>& dst, const LocalTensor<T>& src0, T src1Scalar, const int32_t& count, const char* intriName)
{
    check::VecBinaryScalarApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src0.GetPhyAddr())),
        static_cast<uint32_t>(sizeof(PrimT<U>)),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint64_t>(dst.GetSize() * sizeof(PrimT<U>)),
        static_cast<uint64_t>(src0.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src0.GetPosition()),
        static_cast<uint32_t>(count)};
    return CheckFuncVecBinaryScalarCmpImpl(chkParams, intriName);
}

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
template <const BinaryConfig& config, typename T, typename U, typename S>
bool CheckFuncVecBinaryScalarCmp(
    const T& dstLocal, const U& src0, const S& src1, const int32_t& count, const char* intriName)
{
    using T1 = typename T::PrimType;
    constexpr int8_t pos = config.scalarTensorIndex;
    if constexpr ((pos == 0) || (pos == 1 && TypeUtils::IsInnerDefaultType<U>())) {
        (void)(src0);
        using S1 = typename S::PrimType;
        check::VecBinaryScalarApiParams chkParams{
            static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dstLocal.GetPhyAddr())),
            static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src1.GetPhyAddr())),
            static_cast<uint32_t>(sizeof(T1)),
            static_cast<uint32_t>(sizeof(S1)),
            static_cast<uint64_t>(dstLocal.GetSize() * sizeof(T1)),
            static_cast<uint64_t>(src1.GetSize() * sizeof(S1)),
            static_cast<uint8_t>(dstLocal.GetPosition()),
            static_cast<uint8_t>(src1.GetPosition()),
            static_cast<uint32_t>(count),
            0};
        return CheckFuncVecBinaryScalarCmpImpl(chkParams, intriName);
    } else if constexpr (pos == 1) {
        (void)(src1);
        using U1 = typename U::PrimType;
        check::VecBinaryScalarApiParams chkParams{
            static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dstLocal.GetPhyAddr())),
            static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src0.GetPhyAddr())),
            static_cast<uint32_t>(sizeof(T1)),
            static_cast<uint32_t>(sizeof(U1)),
            static_cast<uint64_t>(dstLocal.GetSize() * sizeof(T1)),
            static_cast<uint64_t>(src0.GetSize() * sizeof(U1)),
            static_cast<uint8_t>(dstLocal.GetPosition()),
            static_cast<uint8_t>(src0.GetPosition()),
            static_cast<uint32_t>(count),
            1};
        return CheckFuncVecBinaryScalarCmpImpl(chkParams, intriName);
    }
}

template <const BinaryConfig& config, typename T, typename U, typename S>
bool CheckFuncVecBinaryScalarCmp(
    const T& dstLocal, const U& src0, const S& src1, const uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams, const char* intriName)
{
    using T1 = typename T::PrimType;
    constexpr int8_t pos = config.scalarTensorIndex;
    if constexpr ((pos == 0) || (pos == 1 && TypeUtils::IsInnerDefaultType<U>())) {
        (void)(src0);
        using S1 = typename S::PrimType;
        check::VecBinaryScalarApiParams chkParams{
            static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dstLocal.GetPhyAddr())),
            static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src1.GetPhyAddr())),
            repeatTime,
            static_cast<uint16_t>(repeatParams.dstBlkStride),
            static_cast<uint16_t>(repeatParams.srcBlkStride),
            static_cast<uint16_t>(repeatParams.dstRepStride),
            static_cast<uint16_t>(repeatParams.srcRepStride),
            static_cast<uint32_t>(sizeof(S1)),
            static_cast<uint32_t>(sizeof(S1)),
            static_cast<uint64_t>(dstLocal.GetSize() * sizeof(T1)),
            static_cast<uint64_t>(src1.GetSize() * sizeof(S1)),
            static_cast<uint8_t>(dstLocal.GetPosition()),
            static_cast<uint8_t>(src1.GetPosition()),
            0};
        return CheckFuncVecBinaryScalarCmpImpl(chkParams, mask, intriName);
    } else if constexpr (pos == 1) {
        (void)(src1);
        using U1 = typename U::PrimType;
        check::VecBinaryScalarApiParams chkParams{
            static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dstLocal.GetPhyAddr())),
            static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src0.GetPhyAddr())),
            repeatTime,
            static_cast<uint16_t>(repeatParams.dstBlkStride),
            static_cast<uint16_t>(repeatParams.srcBlkStride),
            static_cast<uint16_t>(repeatParams.dstRepStride),
            static_cast<uint16_t>(repeatParams.srcRepStride),
            static_cast<uint32_t>(sizeof(U1)),
            static_cast<uint32_t>(sizeof(U1)),
            static_cast<uint64_t>(dstLocal.GetSize() * sizeof(T1)),
            static_cast<uint64_t>(src0.GetSize() * sizeof(U1)),
            static_cast<uint8_t>(dstLocal.GetPosition()),
            static_cast<uint8_t>(src0.GetPosition()),
            1};
        return CheckFuncVecBinaryScalarCmpImpl(chkParams, mask, intriName);
    }
}
#endif

template <typename T, typename U>
check::VecBinaryScalarApiParams BuildVecUnaryParams(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
    (void)(scalarValue);
    return check::VecBinaryScalarApiParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        repeatTime,
        static_cast<uint16_t>(repeatParams.dstBlkStride),
        static_cast<uint16_t>(repeatParams.srcBlkStride),
        static_cast<uint16_t>(repeatParams.dstRepStride),
        static_cast<uint16_t>(repeatParams.srcRepStride),
        static_cast<uint32_t>(sizeof(PrimType)),
        static_cast<uint32_t>(sizeof(PrimType)),
        static_cast<uint64_t>(dst.GetSize() * sizeof(PrimType)),
        static_cast<uint64_t>(src.GetSize() * sizeof(PrimType)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src.GetPosition())};
}

template <typename T, typename U>
bool CheckFunVecBinaryScalar(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams, const char* intriName)
{
    auto chkParams = BuildVecUnaryParams(dst, src, scalarValue, repeatTime, repeatParams);
    return CheckFunVecBinaryScalarImplForMaskArray(chkParams, mask, intriName);
}

template <typename T, typename U>
bool CheckFunVecBinaryScalar(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const uint64_t mask,
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams, const char* intriName)
{
    auto chkParams = BuildVecUnaryParams(dst, src, scalarValue, repeatTime, repeatParams);
    return CheckFunVecBinaryScalarImpl(chkParams, mask, intriName);
}

template <typename T, typename U>
check::VecBinaryScalarApiParams BuildVecBinaryParams(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams)
{
    using PrimType = PrimT<T>;
    (void)(scalarValue);
    return check::VecBinaryScalarApiParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        repeatTime,
        static_cast<uint16_t>(repeatParams.dstBlkStride),
        static_cast<uint16_t>(repeatParams.src0BlkStride),
        static_cast<uint16_t>(repeatParams.dstRepStride),
        static_cast<uint16_t>(repeatParams.src0RepStride),
        static_cast<uint32_t>(sizeof(PrimType)),
        static_cast<uint32_t>(sizeof(PrimType)),
        static_cast<uint64_t>(dst.GetSize() * sizeof(PrimType)),
        static_cast<uint64_t>(src.GetSize() * sizeof(PrimType)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src.GetPosition())};
}

template <typename T, typename U>
bool CheckFunVecBinaryScalar(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const uint64_t mask[],
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams, const char* intriName)
{
    auto chkParams = BuildVecBinaryParams(dst, src, scalarValue, repeatTime, repeatParams);
    return CheckFunVecBinaryScalarImplForMaskArray(chkParams, mask, intriName);
}

template <typename T, typename U>
bool CheckFunVecBinaryScalar(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const uint64_t mask,
    const uint8_t repeatTime, const BinaryRepeatParams& repeatParams, const char* intriName)
{
    auto chkParams = BuildVecBinaryParams(dst, src, scalarValue, repeatTime, repeatParams);
    return CheckFunVecBinaryScalarImpl(chkParams, mask, intriName);
}

template <typename T, typename U>
bool CheckFunVecBinaryScalar(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const int32_t& count,
    const char* intriName)
{
    using PrimType = PrimT<T>;
    (void)(scalarValue);
    check::VecBinaryScalarApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        static_cast<uint32_t>(sizeof(PrimType)),
        static_cast<uint32_t>(sizeof(PrimType)),
        static_cast<uint64_t>(dst.GetSize() * sizeof(PrimType)),
        static_cast<uint64_t>(src.GetSize() * sizeof(PrimType)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src.GetPosition()),
        static_cast<uint32_t>(count)};
    return CheckFunVecBinaryScalarImpl(chkParams, intriName);
}

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113)
template <const BinaryConfig& config, typename T, typename S>
check::VecBinaryScalarApiParams BuildVecUnaryParamsPos0(
    const T& dstLocal, const S& src1, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
    using ActualT = typename T::PrimType;
    return check::VecBinaryScalarApiParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dstLocal.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src1.GetPhyAddr())),
        repeatTime,
        static_cast<uint16_t>(repeatParams.dstBlkStride),
        static_cast<uint16_t>(repeatParams.srcBlkStride),
        static_cast<uint16_t>(repeatParams.dstRepStride),
        static_cast<uint16_t>(repeatParams.srcRepStride),
        static_cast<uint32_t>(sizeof(ActualT)),
        static_cast<uint32_t>(sizeof(ActualT)),
        static_cast<uint64_t>(dstLocal.GetSize() * sizeof(ActualT)),
        static_cast<uint64_t>(src1.GetSize() * sizeof(ActualT)),
        static_cast<uint8_t>(dstLocal.GetPosition()),
        static_cast<uint8_t>(src1.GetPosition()),
        0};
}

template <const BinaryConfig& config, typename T, typename U>
check::VecBinaryScalarApiParams BuildVecUnaryParamsPos1(
    const T& dstLocal, const U& src0, const uint8_t repeatTime, const UnaryRepeatParams& repeatParams)
{
    using ActualT = typename T::PrimType;
    return check::VecBinaryScalarApiParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dstLocal.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src0.GetPhyAddr())),
        repeatTime,
        static_cast<uint16_t>(repeatParams.dstBlkStride),
        static_cast<uint16_t>(repeatParams.srcBlkStride),
        static_cast<uint16_t>(repeatParams.dstRepStride),
        static_cast<uint16_t>(repeatParams.srcRepStride),
        static_cast<uint32_t>(sizeof(ActualT)),
        static_cast<uint32_t>(sizeof(ActualT)),
        static_cast<uint64_t>(dstLocal.GetSize() * sizeof(ActualT)),
        static_cast<uint64_t>(src0.GetSize() * sizeof(ActualT)),
        static_cast<uint8_t>(dstLocal.GetPosition()),
        static_cast<uint8_t>(src0.GetPosition()),
        1};
}

template <const BinaryConfig& config, typename T, typename U, typename S>
bool CheckFunVecBinaryScalar(
    const T& dstLocal, const U& src0, const S& src1, const uint64_t mask[2], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams, const char* intriName)
{
    using ActualT = typename T::PrimType;
    constexpr int8_t pos = config.scalarTensorIndex;
    if constexpr ((pos == 0) || (pos == 1 && IsSameType<ActualT, U>::value)) {
        auto chkParams = BuildVecUnaryParamsPos0<config>(dstLocal, src1, repeatTime, repeatParams);
        return CheckFunVecBinaryScalarImplForMaskArray(chkParams, mask, intriName);
    } else if constexpr (pos == 1) {
        auto chkParams = BuildVecUnaryParamsPos1<config>(dstLocal, src0, repeatTime, repeatParams);
        return CheckFunVecBinaryScalarImplForMaskArray(chkParams, mask, intriName);
    }
}

template <const BinaryConfig& config, typename T, typename U, typename S>
bool CheckFunVecBinaryScalar(
    const T& dstLocal, const U& src0, const S& src1, const uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams, const char* intriName)
{
    using ActualT = typename T::PrimType;
    constexpr int8_t pos = config.scalarTensorIndex;
    if constexpr ((pos == 0) || (pos == 1 && IsSameType<ActualT, U>::value)) {
        auto chkParams = BuildVecUnaryParamsPos0<config>(dstLocal, src1, repeatTime, repeatParams);
        return CheckFunVecBinaryScalarImpl(chkParams, mask, intriName);
    } else if constexpr (pos == 1) {
        auto chkParams = BuildVecUnaryParamsPos1<config>(dstLocal, src0, repeatTime, repeatParams);
        return CheckFunVecBinaryScalarImpl(chkParams, mask, intriName);
    }
}

template <const BinaryConfig& config, typename T, typename U, typename S>
bool CheckFunVecBinaryScalar(
    const T& dstLocal, const U& src0, const S& src1, const int32_t& count, const char* intriName)
{
    using ActualT = typename T::PrimType;
    constexpr int8_t pos = config.scalarTensorIndex;
    if constexpr ((pos == 0) || (pos == 1 && IsSameType<ActualT, U>::value)) {
        check::VecBinaryScalarApiParams chkParams{
            static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dstLocal.GetPhyAddr())),
            static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src1.GetPhyAddr())),
            static_cast<uint32_t>(sizeof(ActualT)),
            static_cast<uint32_t>(sizeof(ActualT)),
            static_cast<uint64_t>(dstLocal.GetSize() * sizeof(ActualT)),
            static_cast<uint64_t>(src1.GetSize() * sizeof(ActualT)),
            static_cast<uint8_t>(dstLocal.GetPosition()),
            static_cast<uint8_t>(src1.GetPosition()),
            static_cast<uint32_t>(count),
            0};
        return CheckFunVecBinaryScalarImpl(chkParams, intriName);
    } else if constexpr (pos == 1) {
        check::VecBinaryScalarApiParams chkParams{
            static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dstLocal.GetPhyAddr())),
            static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src0.GetPhyAddr())),
            static_cast<uint32_t>(sizeof(ActualT)),
            static_cast<uint32_t>(sizeof(ActualT)),
            static_cast<uint64_t>(dstLocal.GetSize() * sizeof(ActualT)),
            static_cast<uint64_t>(src0.GetSize() * sizeof(ActualT)),
            static_cast<uint8_t>(dstLocal.GetPosition()),
            static_cast<uint8_t>(src0.GetPosition()),
            static_cast<uint32_t>(count),
            1};
        return CheckFunVecBinaryScalarImpl(chkParams, intriName);
    }
}

template <const BinaryConfig& config, typename T, typename S>
check::VecBinaryScalarApiParams BuildVecBinaryParamsPos0(
    const T& dstLocal, const S& src1, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using ActualT = typename T::PrimType;
    return check::VecBinaryScalarApiParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dstLocal.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src1.GetPhyAddr())),
        repeatTime,
        static_cast<uint16_t>(repeatParams.dstBlkStride),
        static_cast<uint16_t>(repeatParams.src1BlkStride),
        static_cast<uint16_t>(repeatParams.dstRepStride),
        static_cast<uint16_t>(repeatParams.src1RepStride),
        static_cast<uint32_t>(sizeof(ActualT)),
        static_cast<uint32_t>(sizeof(ActualT)),
        static_cast<uint64_t>(dstLocal.GetSize() * sizeof(ActualT)),
        static_cast<uint64_t>(src1.GetSize() * sizeof(ActualT)),
        static_cast<uint8_t>(dstLocal.GetPosition()),
        static_cast<uint8_t>(src1.GetPosition()),
        0};
}

template <const BinaryConfig& config, typename T, typename U>
check::VecBinaryScalarApiParams BuildVecBinaryParamsPos1(
    const T& dstLocal, const U& src0, const uint8_t repeatTime, const BinaryRepeatParams& repeatParams)
{
    using ActualT = typename T::PrimType;
    return check::VecBinaryScalarApiParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dstLocal.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src0.GetPhyAddr())),
        repeatTime,
        static_cast<uint16_t>(repeatParams.dstBlkStride),
        static_cast<uint16_t>(repeatParams.src0BlkStride),
        static_cast<uint16_t>(repeatParams.dstRepStride),
        static_cast<uint16_t>(repeatParams.src0RepStride),
        static_cast<uint32_t>(sizeof(ActualT)),
        static_cast<uint32_t>(sizeof(ActualT)),
        static_cast<uint64_t>(dstLocal.GetSize() * sizeof(ActualT)),
        static_cast<uint64_t>(src0.GetSize() * sizeof(ActualT)),
        static_cast<uint8_t>(dstLocal.GetPosition()),
        static_cast<uint8_t>(src0.GetPosition()),
        1};
}

template <const BinaryConfig& config, typename T, typename U, typename S>
bool CheckFunVecBinaryScalar(
    const T& dstLocal, const U& src0, const S& src1, const uint64_t mask[2], const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams, const char* intriName)
{
    using ActualT = typename T::PrimType;
    constexpr int8_t pos = config.scalarTensorIndex;
    if constexpr ((pos == 0) || (pos == 1 && IsSameType<ActualT, U>::value)) {
        auto chkParams = BuildVecBinaryParamsPos0<config>(dstLocal, src1, repeatTime, repeatParams);
        return CheckFunVecBinaryScalarImplForMaskArray(chkParams, mask, intriName);
    } else if constexpr (pos == 1) {
        auto chkParams = BuildVecBinaryParamsPos1<config>(dstLocal, src0, repeatTime, repeatParams);
        return CheckFunVecBinaryScalarImplForMaskArray(chkParams, mask, intriName);
    }
}

template <const BinaryConfig& config, typename T, typename U, typename S>
bool CheckFunVecBinaryScalar(
    const T& dstLocal, const U& src0, const S& src1, const uint64_t mask, const uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams, const char* intriName)
{
    using ActualT = typename T::PrimType;
    constexpr int8_t pos = config.scalarTensorIndex;
    if constexpr ((pos == 0) || (pos == 1 && IsSameType<ActualT, U>::value)) {
        auto chkParams = BuildVecBinaryParamsPos0<config>(dstLocal, src1, repeatTime, repeatParams);
        return CheckFunVecBinaryScalarImpl(chkParams, mask, intriName);
    } else if constexpr (pos == 1) {
        auto chkParams = BuildVecBinaryParamsPos1<config>(dstLocal, src0, repeatTime, repeatParams);
        return CheckFunVecBinaryScalarImpl(chkParams, mask, intriName);
    }
}
#endif

template <typename T, typename U>
check::VecBinaryScalarApiParams BuildVecBinaryParamsDiffType(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const PrimT<U>& scalarValue, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams)
{
    (void)(scalarValue);
    return check::VecBinaryScalarApiParams{
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
        static_cast<uint8_t>(src.GetPosition())};
}

template <typename T, typename U>
bool CheckFunVecBinaryScalarDiffType(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const PrimT<U>& scalarValue, const uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams, const char* intriName)
{
    auto chkParams = BuildVecBinaryParamsDiffType(dst, src, scalarValue, repeatTime, repeatParams);
    return CheckFunVecBinaryScalarImplForMaskArray(chkParams, mask, intriName);
}

template <typename T, typename U>
bool CheckFunVecBinaryScalarDiffType(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const PrimT<U>& scalarValue, const uint64_t mask,
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams, const char* intriName)
{
    auto chkParams = BuildVecBinaryParamsDiffType(dst, src, scalarValue, repeatTime, repeatParams);
    return CheckFunVecBinaryScalarImpl(chkParams, mask, intriName);
}

template <typename T, typename U>
bool CheckFunVecBinaryScalarDiffType(
    const LocalTensor<T>& dst, const LocalTensor<U>& src, const PrimT<U>& scalarValue, const int32_t& count,
    const char* intriName)
{
    (void)(scalarValue);
    check::VecBinaryScalarApiParams chkParams{
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(dst.GetPhyAddr())),
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(src.GetPhyAddr())),
        static_cast<uint32_t>(sizeof(PrimT<T>)),
        static_cast<uint32_t>(sizeof(PrimT<U>)),
        static_cast<uint64_t>(dst.GetSize() * sizeof(PrimT<T>)),
        static_cast<uint64_t>(src.GetSize() * sizeof(PrimT<U>)),
        static_cast<uint8_t>(dst.GetPosition()),
        static_cast<uint8_t>(src.GetPosition()),
        static_cast<uint32_t>(count)};
    return CheckFunVecBinaryScalarImpl(chkParams, intriName);
}
} // namespace AscendC
#endif

#endif
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_CHECK_VEC_BINARY_SCALAR_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_CHECK_VEC_BINARY_SCALAR_H__
#endif
