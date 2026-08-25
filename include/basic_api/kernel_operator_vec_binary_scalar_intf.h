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
 * \file kernel_operator_vec_binary_scalar_intf.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "kernel_operator_vec_binary_scalar_intf.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_SCALAR_INTF_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_VEC_BINARY_SCALAR_INTERFACE_H
#define ASCENDC_MODULE_OPERATOR_VEC_BINARY_SCALAR_INTERFACE_H

#include "../../impl/basic_api/kernel_macros.h"
#include "kernel_tensor.h"
#include "kernel_struct_unary.h"
#include "kernel_struct_binary.h"
#include "../../impl/basic_api/utils/kernel_utils_macros.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstdint>
#include "stub_def.h"
#endif

#pragma begin_pipe(V)
namespace AscendC {
/* **************************************************************************************************
 * Adds                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Adds Level 0
 * @brief dst[i] = src[i] + sacalar
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] repeatParams.dstBlkStride dst block stride
 * @param [in] repeatParams.srcBlkStride src block stride
 * @param [in] repeatParams.dstRepStride dst repeat stride
 * @param [in] repeatParams.src0RepStride src repeat stride
 */
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(3510, "Adds is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Adds(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams);

template <
    typename T, typename U, bool isSetMask = true,
    typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__ASC_USE_RESERVED_UBUF__(3510, "Adds is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Adds(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams);

template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(3510, "Adds is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Adds(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

template <
    typename T, typename U, bool isSetMask = true,
    typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__ASC_USE_RESERVED_UBUF__(3510, "Adds is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Adds(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113)
// One of src0/src1 should be scalar or single point LocalTensor
template <
    typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG,
    typename U, typename S, typename V>
__ASC_USE_RESERVED_UBUF__(3510, "Adds is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Adds(
    const U& dst, const S& src0, const V& src1, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

template <
    typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG,
    typename U, typename S, typename V>
__ASC_USE_RESERVED_UBUF__(3510, "Adds is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Adds(
    const U& dst, const S& src0, const V& src1, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);
#endif

/*
 * @ingroup Adds Level 2
 * @brief dst = src[i] + sacalar
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, bool isSetMask = true>
__aicore__ inline void Adds(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, const int32_t& count);

template <
    typename T, typename U, bool isSetMask = true,
    typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__aicore__ inline void Adds(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const int32_t& count);

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113)
// One of src0/src1 should be scalar or single point LocalTensor
template <
    typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG,
    typename U, typename S, typename V>
__aicore__ inline void Adds(const U& dst, const S& src0, const V& src1, const int32_t& count);
#endif

/* **************************************************************************************************
 * Muls                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Muls Level 0
 * @brief dst[i] = src[i] * sacalar
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] repeatParams.dstBlkStride dst block stride
 * @param [in] repeatParams.srcBlkStride src block stride
 * @param [in] repeatParams.dstRepStride dst repeat stride
 * @param [in] repeatParams.src0RepStride src repeat stride
 */
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(3510, "Muls is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Muls(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams);

template <
    typename T, typename U, bool isSetMask = true,
    typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__ASC_USE_RESERVED_UBUF__(3510, "Muls is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Muls(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams);

template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(3510, "Muls is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Muls(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

template <
    typename T, typename U, bool isSetMask = true,
    typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__ASC_USE_RESERVED_UBUF__(3510, "Muls is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Muls(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113)
// One of src0/src1 should be scalar or single point LocalTensor
template <
    typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG,
    typename U, typename S, typename V>
__ASC_USE_RESERVED_UBUF__(3510, "Muls is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Muls(
    const U& dst, const S& src0, const V& src1, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

template <
    typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG,
    typename U, typename S, typename V>
__ASC_USE_RESERVED_UBUF__(3510, "Muls is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Muls(
    const U& dst, const S& src0, const V& src1, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);
#endif

/*
 * @ingroup Muls Level 2
 * @brief dst = src[i] * sacalar
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, bool isSetMask = true>
__aicore__ inline void Muls(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, const int32_t& count);

template <
    typename T, typename U, bool isSetMask = true,
    typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__aicore__ inline void Muls(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const int32_t& count);

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113)
// One of src0/src1 should be scalar or single point LocalTensor
template <
    typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG,
    typename U, typename S, typename V>
__aicore__ inline void Muls(const U& dst, const S& src0, const V& src1, const int32_t& count);
#endif

/* **************************************************************************************************
 * Maxs                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Maxs Level 0
 * @brief dst[i] = src[i] > sacalar ? src[0] : scalar
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] repeatParams.dstBlkStride dst block stride
 * @param [in] repeatParams.srcBlkStride src block stride
 * @param [in] repeatParams.dstRepStride dst repeat stride
 * @param [in] repeatParams.src0RepStride src repeat stride
 */
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(3510, "Maxs is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Maxs(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams);

template <
    typename T, typename U, bool isSetMask = true,
    typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__ASC_USE_RESERVED_UBUF__(3510, "Maxs is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Maxs(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams);

template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(3510, "Maxs is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Maxs(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

template <
    typename T, typename U, bool isSetMask = true,
    typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__ASC_USE_RESERVED_UBUF__(3510, "Maxs is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Maxs(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113)
// One of src0/src1 should be scalar or single point LocalTensor
template <
    typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG,
    typename U, typename S, typename V>
__ASC_USE_RESERVED_UBUF__(3510, "Maxs is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Maxs(
    const U& dst, const S& src0, const V& src1, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

template <
    typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG,
    typename U, typename S, typename V>
__ASC_USE_RESERVED_UBUF__(3510, "Maxs is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Maxs(
    const U& dst, const S& src0, const V& src1, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);
#endif

/*
 * @ingroup Maxs Level 2
 * @brief dst = src[i] > sacalar ? src[0] : scalar
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, bool isSetMask = true>
__aicore__ inline void Maxs(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, const int32_t& count);

template <
    typename T, typename U, bool isSetMask = true,
    typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__aicore__ inline void Maxs(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const int32_t& count);

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113)
// One of src0/src1 should be scalar or single point LocalTensor
template <
    typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG,
    typename U, typename S, typename V>
__aicore__ inline void Maxs(const U& dst, const S& src0, const V& src1, const int32_t& count);
#endif

/* **************************************************************************************************
 * Mins                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Mins Level 0
 * @brief dst[i] = src[i] < sacalar ? src[0] : scalar
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] repeatParams.dstBlkStride dst block stride
 * @param [in] repeatParams.srcBlkStride src block stride
 * @param [in] repeatParams.dstRepStride dst repeat stride
 * @param [in] repeatParams.src0RepStride src repeat stride
 */
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(3510, "Mins is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Mins(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams);

template <
    typename T, typename U, bool isSetMask = true,
    typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__ASC_USE_RESERVED_UBUF__(3510, "Mins is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Mins(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams);

template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(3510, "Mins is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Mins(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

template <
    typename T, typename U, bool isSetMask = true,
    typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__ASC_USE_RESERVED_UBUF__(3510, "Mins is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Mins(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113)
// One of src0/src1 should be scalar or single point LocalTensor
template <
    typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG,
    typename U, typename S, typename V>
__ASC_USE_RESERVED_UBUF__(3510, "Mins is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Mins(
    const U& dst, const S& src0, const V& src1, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

template <
    typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG,
    typename U, typename S, typename V>
__ASC_USE_RESERVED_UBUF__(3510, "Mins is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Mins(
    const U& dst, const S& src0, const V& src1, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);
#endif

/*
 * @ingroup Mins Level 2
 * @brief dst = src[i] < sacalar ? src[0] : scalar
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, bool isSetMask = true>
__aicore__ inline void Mins(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, const int32_t& count);

template <
    typename T, typename U, bool isSetMask = true,
    typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__aicore__ inline void Mins(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const int32_t& count);

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113)
// One of src0/src1 should be scalar or single point LocalTensor
template <
    typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG,
    typename U, typename S, typename V>
__aicore__ inline void Mins(const U& dst, const S& src0, const V& src1, const int32_t& count);
#endif

/* **************************************************************************************************
 * ShiftLeft                                             *
 * ************************************************************************************************* */
/*
 * @ingroup ShiftLeft Level 0
 * @brief dst[i] = src[i] << sacalar
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] repeatParams.dstBlkStride dst block stride
 * @param [in] repeatParams.srcBlkStride src block stride
 * @param [in] repeatParams.dstRepStride dst repeat stride
 * @param [in] repeatParams.src0RepStride src repeat stride
 */
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(3510, "ShiftLeft is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void ShiftLeft(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams);

template <
    typename T, typename U, bool isSetMask = true,
    typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__ASC_USE_RESERVED_UBUF__(3510, "ShiftLeft is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void ShiftLeft(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams);

template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(3510, "ShiftLeft is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void ShiftLeft(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

template <
    typename T, typename U, bool isSetMask = true,
    typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__ASC_USE_RESERVED_UBUF__(3510, "ShiftLeft is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void ShiftLeft(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

/*
 * @ingroup Shiftleft Level 2
 * @brief dst = src[i] << sacalar
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, bool isSetMask = true>
__aicore__ inline void ShiftLeft(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, const int32_t& count);

template <
    typename T, typename U, bool isSetMask = true,
    typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__aicore__ inline void ShiftLeft(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const int32_t& count);

/* **************************************************************************************************
 * ShiftRight                                             *
 * ************************************************************************************************* */
/*
 * @ingroup ShiftRight Level 0
 * @brief dst[i] = src[i] >> sacalar
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] repeatParams.dstBlkStride dst block stride
 * @param [in] repeatParams.srcBlkStride src block stride
 * @param [in] repeatParams.dstRepStride dst repeat stride
 * @param [in] repeatParams.src0RepStride src repeat stride
 */
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "ShiftRight is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void ShiftRight(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams, bool roundEn = false);

template <
    typename T, typename U, bool isSetMask = true,
    typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "ShiftRight is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void ShiftRight(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams, bool roundEn);

template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "ShiftRight is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void ShiftRight(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams, bool roundEn = false);

template <
    typename T, typename U, bool isSetMask = true,
    typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "ShiftRight is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void ShiftRight(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams, bool roundEn);

/*
 * @ingroup ShiftRight Level 2
 * @brief dst = src[i] >> sacalar
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, bool isSetMask = true>
__aicore__ inline void ShiftRight(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, const int32_t& count);

template <
    typename T, typename U, bool isSetMask = true,
    typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__aicore__ inline void ShiftRight(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const int32_t& count);

/* **************************************************************************************************
 * LeakyRelu                                             *
 * ************************************************************************************************* */
/*
 * @ingroup LeakyRelu Level 0
 * @brief dst[i] = src[i] < 0 ? (scalar * src[i]) : src[i]
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] repeatParams.dstBlkStride dst block stride
 * @param [in] repeatParams.srcBlkStride src block stride
 * @param [in] repeatParams.dstRepStride dst repeat stride
 * @param [in] repeatParams.src0RepStride src repeat stride
 */
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(3510, "LeakyRelu is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void LeakyRelu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams);

template <
    typename T, typename U, bool isSetMask = true,
    typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__ASC_USE_RESERVED_UBUF__(3510, "LeakyRelu is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void LeakyRelu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask[],
    const uint8_t repeatTime, const UnaryRepeatParams& repeatParams);

template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(3510, "LeakyRelu is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void LeakyRelu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

template <
    typename T, typename U, bool isSetMask = true,
    typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__ASC_USE_RESERVED_UBUF__(3510, "LeakyRelu is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void LeakyRelu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

/*
 * @ingroup LeakyRelu Level 2
 * @brief dst = src[i] < 0 ? (scalar * src[i]) : src[i]
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, bool isSetMask = true>
__aicore__ inline void LeakyRelu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const T& scalarValue, const int32_t& count);

template <
    typename T, typename U, bool isSetMask = true,
    typename Std::enable_if<Std::is_same<PrimT<T>, U>::value, bool>::type = true>
__aicore__ inline void LeakyRelu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, const U& scalarValue, const int32_t& count);

/* **************************************************************************************************
 * Subs                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Subs Level 0
 * @brief dst[i] = src0 - src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor or scalar, one of src0/src1 should be scalar or single point LocalTensor
 * @param [in] src1 input LocalTensor or scalar, one of src0/src1 should be scalar or single point LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.srcBlkStride src block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src repeat stride
 */
template <
    typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG,
    typename U, typename S, typename V>
__ASC_USE_RESERVED_UBUF__(3510, "Subs is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Subs(
    const U& dst, const S& src0, const V& src1, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

template <
    typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG,
    typename U, typename S, typename V>
__ASC_USE_RESERVED_UBUF__(3510, "Subs is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Subs(
    const U& dst, const S& src0, const V& src1, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

/*
 * @ingroup Subs Level 2
 * @brief dst = src0 - src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor or scalar, one of src0/src1 should be scalar or single point LocalTensor
 * @param [in] src1 input LocalTensor or scalar, one of src0/src1 should be scalar or single point LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <
    typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG,
    typename U, typename S, typename V>
__aicore__ inline void Subs(const U& dst, const S& src0, const V& src1, const int32_t& count);

/* **************************************************************************************************
 * Divs                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Divs Level 0
 * @brief dst[i] = src0 // src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor or scalar, one of src0/src1 should be scalar or single point LocalTensor
 * @param [in] src1 input LocalTensor or scalar, one of src0/src1 should be scalar or single point LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.srcBlkStride src block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src repeat stride
 */
template <
    typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG,
    typename U, typename S, typename V>
__ASC_USE_RESERVED_UBUF__(3510, "Divs is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Divs(
    const U& dst, const S& src0, const V& src1, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

template <
    typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG,
    typename U, typename S, typename V>
__ASC_USE_RESERVED_UBUF__(3510, "Divs is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Divs(
    const U& dst, const S& src0, const V& src1, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

/*
 * @ingroup Divs Level 2
 * @brief dst = src0 // src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor or scalar, one of src0/src1 should be scalar or single point LocalTensor
 * @param [in] src1 input LocalTensor or scalar, one of src0/src1 should be scalar or single point LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <
    typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG,
    typename U, typename S, typename V>
__aicore__ inline void Divs(const U& dst, const S& src0, const V& src1, const int32_t& count);

/* **************************************************************************************************
 * Ands                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Ands Level 0
 * @brief dst[i] = src0 & src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor or scalar, one of src0/src1 should be scalar or single point LocalTensor
 * @param [in] src1 input LocalTensor or scalar, one of src0/src1 should be scalar or single point LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.srcBlkStride src block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src repeat stride
 */
template <
    typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG,
    typename U, typename S, typename V>
__ASC_USE_RESERVED_UBUF__(3510, "Ands is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Ands(
    const U& dst, const S& src0, const V& src1, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

template <
    typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG,
    typename U, typename S, typename V>
__ASC_USE_RESERVED_UBUF__(3510, "Ands is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Ands(
    const U& dst, const S& src0, const V& src1, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

/*
 * @ingroup Ands Level 2
 * @brief dst = src0 & src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor or scalar, one of src0/src1 should be scalar or single point LocalTensor
 * @param [in] src1 input LocalTensor or scalar, one of src0/src1 should be scalar or single point LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <
    typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG,
    typename U, typename S, typename V>
__aicore__ inline void Ands(const U& dst, const S& src0, const V& src1, const int32_t& count);

/* **************************************************************************************************
 * Ors                                             *
 * ************************************************************************************************* */
/*
 * @ingroup Ors Level 0
 * @brief dst[i] = src0 | src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor or scalar, one of src0/src1 should be scalar or single point LocalTensor
 * @param [in] src1 input LocalTensor or scalar, one of src0/src1 should be scalar or single point LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] intriParams.dstBlkStride dst block stride
 * @param [in] intriParams.srcBlkStride src block stride
 * @param [in] intriParams.dstRepStride dst repeat stride
 * @param [in] intriParams.src0RepStride src repeat stride
 */
template <
    typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG,
    typename U, typename S, typename V>
__ASC_USE_RESERVED_UBUF__(3510, "Ors is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Ors(
    const U& dst, const S& src0, const V& src1, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

template <
    typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG,
    typename U, typename S, typename V>
__ASC_USE_RESERVED_UBUF__(3510, "Ors is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Ors(
    const U& dst, const S& src0, const V& src1, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

/*
 * @ingroup Ors Level 2
 * @brief dst = src0 | src1
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor or scalar, one of src0/src1 should be scalar or single point LocalTensor
 * @param [in] src1 input LocalTensor or scalar, one of src0/src1 should be scalar or single point LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <
    typename T = BinaryDefaultType, bool isSetMask = true, const BinaryConfig& config = DEFAULT_BINARY_CONFIG,
    typename U, typename S, typename V>
__aicore__ inline void Ors(const U& dst, const S& src0, const V& src1, const int32_t& count);

/* **************************************************************************************************
 * MulsCast                                             *
 * ************************************************************************************************* */
/*
 * @ingroup MulsCast Level 2
 * @brief dst = abs(src0 - src1)
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <
    typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType,
    const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
__aicore__ inline void MulsCast(const T2& dst, const T3& src0, const T4& src1, const uint32_t count);

// FusedMulsCast has been updated, please use MulsCast instead.
template <
    typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType,
    const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
__aicore__ inline void FusedMulsCast(const T2& dst, const T3& src0, const T4& src1, const uint32_t count);

} // namespace AscendC
#pragma end_pipe
#if defined(__NPU_ARCH__)
#include "../../impl/basic_api/kernel_operator_vec_binary_scalar_intf_impl.h"
#endif
#endif // ASCENDC_MODULE_OPERATOR_VEC_BINARY_SCALAR_INTERFACE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_SCALAR_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_BINARY_SCALAR_INTF_H__
#endif
