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
 * \file kernel_operator_vec_cmpsel_intf.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "kernel_operator_vec_cmpsel_intf.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_CMPSEL_INTF_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_VEC_CMPSEL_INTERFACE_H
#define ASCENDC_MODULE_OPERATOR_VEC_CMPSEL_INTERFACE_H

#include "../../impl/basic_api/kernel_macros.h"
#include "kernel_tensor.h"
#include "kernel_struct_binary.h"
#include "kernel_struct_unary.h"
#include "../../impl/basic_api/utils/kernel_utils_mode.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstdint>
#include "stub_def.h"
#endif

#pragma begin_pipe(V)
namespace AscendC {
/* **************************************************************************************************
 * Compare                                           *
 * ************************************************************************************************* */
/*
 * @ingroup Compare Level 0
 * @brief Compare the size of two tensors one by one. If true, the corresponding bit is 1, otherwise it is 0
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] cmpMode compare mode
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] repeatParams.dstBlkStride dst block stride
 * @param [in] repeatParams.src0BlkStride src0 block stride
 * @param [in] repeatParams.src1BlkStride src1 block stride
 * @param [in] repeatParams.dstRepStride dst repeat stride
 * @param [in] repeatParams.src0RepStride src0 repeat stride
 * @param [in] repeatParams.src1RepStride src1 repeat stride
 */
template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void Compare(
    const LocalTensor<U>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, CMPMODE cmpMode,
    const uint64_t mask[], uint8_t repeatTime, const BinaryRepeatParams& repeatParams);

template <typename T, typename U, bool isSetMask = true>
__aicore__ inline void Compare(
    const LocalTensor<U>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, CMPMODE cmpMode,
    const uint64_t mask, uint8_t repeatTime, const BinaryRepeatParams& repeatParams);

template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(3510, "Compare is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Compare(
    const LocalTensor<T>& src0, const LocalTensor<T>& src1, CMPMODE cmpMode, const uint64_t mask[],
    const BinaryRepeatParams& repeatParams);

template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(3510, "Compare is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Compare(
    const LocalTensor<T>& src0, const LocalTensor<T>& src1, CMPMODE cmpMode, const uint64_t mask,
    const BinaryRepeatParams& repeatParams);

/*
 * @ingroup Compare Level 2
 * @brief Compare the size of two tensors one by one. If true, the corresponding bit is 1, otherwise it is 0
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] cmpMode compare mode
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, typename U>
__aicore__ inline void Compare(
    const LocalTensor<U>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, CMPMODE cmpMode, uint32_t count);

template <typename T>
__aicore__ inline void GetCmpMask(const LocalTensor<T>& dst);

template <typename T>
__aicore__ inline void SetCmpMask(const LocalTensor<T>& src);

/* **************************************************************************************************
 * Compares                                           *
 * ************************************************************************************************* */
/*
 * @ingroup Compares Level 0
 * @brief Compare the size of a tensor and a scalar one by one. If true, the corresponding bit is 1, otherwise it is 0
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1Scalar input Scalar
 * @param [in] cmpMode compare mode
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] repeatParams.dstBlkStride dst block stride
 * @param [in] repeatParams.srcBlkStride src0 block stride
 * @param [in] repeatParams.dstRepStride dst repeat stride
 * @param [in] repeatParams.srcRepStride src0 repeat stride
 */
template <typename T, typename U, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(3510, "Compares is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Compares(
    const LocalTensor<U>& dst, const LocalTensor<T>& src0, const T src1Scalar, CMPMODE cmpMode, const uint64_t mask[],
    uint8_t repeatTime, const UnaryRepeatParams& repeatParams);

template <typename T, typename U, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(3510, "Compares is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Compares(
    const LocalTensor<U>& dst, const LocalTensor<T>& src0, const T src1Scalar, CMPMODE cmpMode, const uint64_t mask,
    uint8_t repeatTime, const UnaryRepeatParams& repeatParams);

// CompareScalar has been updated, please use Compares instead.
template <typename T, typename U, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "CompareScalar is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void CompareScalar(
    const LocalTensor<U>& dst, const LocalTensor<T>& src0, const T src1Scalar, CMPMODE cmpMode, const uint64_t mask[],
    uint8_t repeatTime, const UnaryRepeatParams& repeatParams);

// CompareScalar has been updated, please use Compares instead.
template <typename T, typename U, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(
    3510, "CompareScalar is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void CompareScalar(
    const LocalTensor<U>& dst, const LocalTensor<T>& src0, const T src1Scalar, CMPMODE cmpMode, const uint64_t mask,
    uint8_t repeatTime, const UnaryRepeatParams& repeatParams);
/*
 * @ingroup Compares Level 2
 * @brief Compares the size of two tensors one by one. If true, the corresponding bit is 1, otherwise it is 0
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1Scalar input Scalar
 * @param [in] cmpMode compare mode
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, typename U>
__aicore__ inline void Compares(
    const LocalTensor<U>& dst, const LocalTensor<T>& src0, const T src1Scalar, CMPMODE cmpMode, uint32_t count);

// CompareScalar has been updated, please use Compares instead.
template <typename T, typename U>
__aicore__ inline void CompareScalar(
    const LocalTensor<U>& dst, const LocalTensor<T>& src0, const T src1Scalar, CMPMODE cmpMode, uint32_t count);

/* **************************************************************************************************
 * Select                                            *
 * ************************************************************************************************* */
// T must be half or Float
// U must be uint8_t

// ================================
/*
 * @ingroup Select Level 0
 * @brief Select element according to the bit value of sel
 * @param [out] dst output LocalTensor
 * @param [in] selMask select mask LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] selMode select mode
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] repeatParams.dstBlkStride dst block stride
 * @param [in] repeatParams.src0BlkStride src0 block stride
 * @param [in] repeatParams.src1BlkStride src1 block stride
 * @param [in] repeatParams.dstRepStride dst repeat stride
 * @param [in] repeatParams.src0RepStride src0 repeat stride
 * @param [in] repeatParams.src1RepStride src1 repeat stride
 */
// select mode: 0/1/2
template <typename T, typename U, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(2201, "Select is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Select(
    const LocalTensor<T>& dst, const LocalTensor<U>& selMask, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
    SELMODE selMode, uint64_t mask[], uint8_t repeatTime, const BinaryRepeatParams& repeatParams);

// select mode: 0/1/2
template <typename T, typename U, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(2201, "Select is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Select(
    const LocalTensor<T>& dst, const LocalTensor<U>& selMask, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
    SELMODE selMode, uint64_t mask, uint8_t repeatTime, const BinaryRepeatParams& repeatParams);

template <typename T, SELMODE selMode>
__ASC_USE_RESERVED_UBUF__(
    2201, 3510, "Select is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Select(
    const LocalTensor<T>& dst, const LocalTensor<T>& src0, const LocalTensor<T>& src1, uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams);

template <typename T, typename U>
__ASC_USE_RESERVED_UBUF__(
    2201, 3510, "Select is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Select(
    const LocalTensor<T>& dst, const LocalTensor<U>& selMask, const LocalTensor<T>& src0, uint8_t repeatTime,
    const BinaryRepeatParams& repeatParams);

/*
 * @ingroup Select Level 2
 * @brief Select element according to the bit value of sel
 * @param [out] dst output LocalTensor
 * @param [in] selMask select mask LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input LocalTensor
 * @param [in] selMode select mode
 * @param [in] count number Number of data involved in calculation
 */
// select mode: 0/1/2
template <typename T, typename U>
__ASC_USE_RESERVED_UBUF__(2201, "Select is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Select(
    const LocalTensor<T>& dst, const LocalTensor<U>& selMask, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
    SELMODE selMode, uint32_t count);

// ================================
/*
 * @ingroup Select Level 0
 * @brief Select element according to the bit value of sel
 * @param [out] dst output LocalTensor
 * @param [in] selMask select mask LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input number
 * @param [in] selMode select mode
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] repeatParams.dstBlkStride dst block stride
 * @param [in] repeatParams.src0BlkStride src0 block stride
 * @param [in] repeatParams.src1BlkStride src1 block stride
 * @param [in] repeatParams.dstRepStride dst repeat stride
 * @param [in] repeatParams.src0RepStride src0 repeat stride
 * @param [in] repeatParams.src1RepStride src1 repeat stride
 */
// select mode: 1
template <typename T, typename U, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(2201, "Select is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Select(
    const LocalTensor<T>& dst, const LocalTensor<U>& selMask, const LocalTensor<T>& src0, T src1, SELMODE selMode,
    uint64_t mask[], uint8_t repeatTime, const BinaryRepeatParams& repeatParams);

// select mode: 1
template <typename T, typename U, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(2201, "Select is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Select(
    const LocalTensor<T>& dst, const LocalTensor<U>& selMask, const LocalTensor<T>& src0, T src1, SELMODE selMode,
    uint64_t mask, uint8_t repeatTime, const BinaryRepeatParams& repeatParams);

// select mode: 1
/*
 * @ingroup Select Level 2
 * @brief Select element according to the bit value of sel
 * @param [out] dst output LocalTensor
 * @param [in] selMask select mask LocalTensor
 * @param [in] src0 input LocalTensor
 * @param [in] src1 input number
 * @param [in] selMode select mode
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, typename U>
__ASC_USE_RESERVED_UBUF__(2201, "Select is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Select(
    const LocalTensor<T>& dst, const LocalTensor<U>& selMask, const LocalTensor<T>& src0, T src1, SELMODE selMode,
    uint32_t count);

/*
 * @ingroup Compares Level 2
 * @brief Compares the size of two tensors one by one. If true, the corresponding bit is 1, otherwise it is 0
 * @param [out] dst output LocalTensor
 * @param [in] src0 input LocalTensor/Scalar, one of src0/src1 should be Scalar or single point LocalTensor
 * @param [in] src1 input LocalTensor/Scalar, one of src0/src1 should be Scalar or single point LocalTensor
 * @param [in] cmpMode compare mode
 * @param [in] count number Number of data involved in calculation
 */
// One of src0/src1 should be Scalar
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113)
template <
    typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, bool isSetMask = true,
    const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
__ASC_USE_RESERVED_UBUF__(3510, "Compares is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Compares(
    const T2& dst, const T3& src0, const T4& src1, CMPMODE cmpMode, const uint64_t mask[], uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

template <
    typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, bool isSetMask = true,
    const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
__ASC_USE_RESERVED_UBUF__(3510, "Compares is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Compares(
    const T2& dst, const T3& src0, const T4& src1, CMPMODE cmpMode, const uint64_t mask, uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

template <
    typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, bool isSetMask = true,
    const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
__aicore__ inline void Compares(const T2& dst, const T3& src0, const T4& src1, CMPMODE cmpMode, uint32_t count);

// CompareScalar has been updated, please use Compares instead.
template <
    typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, bool isSetMask = true,
    const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
__ASC_USE_RESERVED_UBUF__(
    3510, "CompareScalar is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void CompareScalar(
    const T2& dst, const T3& src0, const T4& src1, CMPMODE cmpMode, const uint64_t mask[], uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

// CompareScalar has been updated, please use Compares instead.
template <
    typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, bool isSetMask = true,
    const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
__ASC_USE_RESERVED_UBUF__(
    3510, "CompareScalar is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void CompareScalar(
    const T2& dst, const T3& src0, const T4& src1, CMPMODE cmpMode, const uint64_t mask, uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

// CompareScalar has been updated, please use Compares instead.
template <
    typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, bool isSetMask = true,
    const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
__aicore__ inline void CompareScalar(const T2& dst, const T3& src0, const T4& src1, CMPMODE cmpMode, uint32_t count);

// select mode 1
// One of src0/src1 should be scalar
template <
    typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, bool isSetMask = true,
    const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
__aicore__ inline void Select(
    const T2& dst, const LocalTensor<T1>& selMask, const T3& src0, const T4& src1, SELMODE selMode, uint64_t mask[],
    uint8_t repeatTime, const BinaryRepeatParams& repeatParams);

template <
    typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType, bool isSetMask = true,
    const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
__aicore__ inline void Select(
    const T2& dst, const LocalTensor<T1>& selMask, const T3& src0, const T4& src1, SELMODE selMode, uint64_t mask,
    uint8_t repeatTime, const BinaryRepeatParams& repeatParams);

template <
    typename T0 = BinaryDefaultType, typename T1 = BinaryDefaultType,
    const BinaryConfig& config = DEFAULT_BINARY_CONFIG, typename T2, typename T3, typename T4>
__aicore__ inline void Select(
    const T2& dst, const LocalTensor<T1>& selMask, const T3& src0, const T4& src1, SELMODE selMode, uint32_t count);
#endif

} // namespace AscendC
#pragma end_pipe

#if defined(__NPU_ARCH__)
#include "../../impl/basic_api/kernel_operator_vec_cmpsel_intf_impl.h"
#endif
#endif // ASCENDC_MODULE_OPERATOR_VEC_CMPSEL_INTERFACE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_CMPSEL_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_CMPSEL_INTF_H__
#endif
