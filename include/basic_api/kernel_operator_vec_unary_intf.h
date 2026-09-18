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
 * \file kernel_operator_vec_unary_intf.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "kernel_operator_vec_unary_intf.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_UNARY_INTF_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_VEC_UNARY_INTERFACE_H
#define ASCENDC_MODULE_OPERATOR_VEC_UNARY_INTERFACE_H

#include "../../impl/basic_api/kernel_macros.h"
#include "kernel_tensor.h"
#include "kernel_struct_unary.h"

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510 || __NPU_ARCH__ == 5102 || __NPU_ARCH__ == 5162)
#include "reg_compute/kernel_reg_compute_utils.h"
#endif

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstdint>
#include "stub_def.h"
#endif

#pragma begin_pipe(V)
namespace AscendC {
/* **************************************************************************************************
 * Unary                                              *
 * ************************************************************************************************* */

/* **************************************** Relu ****************************************** */
/*
 * @ingroup Relu Level 0
 * @brief dst[i] = (src[i] < 0) ? 0 : src[i]
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] repeatParams.dstBlkStride dst block stride
 * @param [in] repeatParams.srcBlkStride src block stride
 * @param [in] repeatParams.dstRepStride dst repeat stride
 * @param [in] repeatParams.src0RepStride src repeat stride
 */
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(3510, "Relu is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Relu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(3510, "Relu is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Relu(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

/*
 * @ingroup Relu Level 2
 * @brief dst[i] = (src[i] < 0) ? 0 : src[i]
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T>
__aicore__ inline void Relu(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count);

/* **************************************** Exp ****************************************** */
/*
 * @ingroup Exp Level 0
 * @brief dst[i] = exp(src[i])
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] repeatParams.dstBlkStride dst block stride
 * @param [in] repeatParams.srcBlkStride src block stride
 * @param [in] repeatParams.dstRepStride dst repeat stride
 * @param [in] repeatParams.src0RepStride src repeat stride
 */
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
template <typename T, bool isSetMask = true, const ExpConfig& config = DEFAULT_EXP_CONFIG>
__ASC_USE_RESERVED_UBUF__(3510, "Exp is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Exp(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);
template <typename T, bool isSetMask = true, const ExpConfig& config = DEFAULT_EXP_CONFIG>
__ASC_USE_RESERVED_UBUF__(3510, "Exp is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Exp(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);
#else
template <typename T, bool isSetMask = true>
__aicore__ inline void Exp(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);
template <typename T, bool isSetMask = true>
__aicore__ inline void Exp(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);
#endif

/*
 * @ingroup Exp Level 2
 * @brief dst[i] = exp(src[i])
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
template <typename T, const ExpConfig& config = DEFAULT_EXP_CONFIG>
__aicore__ inline void Exp(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count);
#else
template <typename T>
__aicore__ inline void Exp(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count);
#endif

/* **************************************** Ln ****************************************** */
/*
 * @ingroup Ln Level 0
 * @brief dst[i] = Ln(src[i])
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] repeatParams.dstBlkStride dst block stride
 * @param [in] repeatParams.srcBlkStride src block stride
 * @param [in] repeatParams.dstRepStride dst repeat stride
 * @param [in] repeatParams.src0RepStride src repeat stride
 */
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
template <typename T, bool isSetMask = true, const LnConfig& config = DEFAULT_LN_CONFIG>
__ASC_USE_RESERVED_UBUF__(3510, "Ln is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Ln(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);
template <typename T, bool isSetMask = true, const LnConfig& config = DEFAULT_LN_CONFIG>
__ASC_USE_RESERVED_UBUF__(3510, "Ln is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Ln(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);
#else
template <typename T, bool isSetMask = true>
__aicore__ inline void Ln(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);
template <typename T, bool isSetMask = true>
__aicore__ inline void Ln(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);
#endif

/*
 * @ingroup Ln Level 2
 * @brief dst[i] = Ln(src[i])
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
template <typename T, const LnConfig& config = DEFAULT_LN_CONFIG>
__aicore__ inline void Ln(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count);
#else
template <typename T>
__aicore__ inline void Ln(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count);
#endif

/* **************************************** Abs ****************************************** */
/*
 * @ingroup Abs Level 0
 * @brief dst[i] = abs(src[i])
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] repeatParams.dstBlkStride dst block stride
 * @param [in] repeatParams.srcBlkStride src block stride
 * @param [in] repeatParams.dstRepStride dst repeat stride
 * @param [in] repeatParams.src0RepStride src repeat stride
 */
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(3510, "Abs is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Abs(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(3510, "Abs is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Abs(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

/*
 * @ingroup Abs Level 2
 * @brief dst[i] = abs(src[i])
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T>
__aicore__ inline void Abs(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count);

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
/*
 * @ingroup Abs Level 2 for complex32/complex64
 * @brief dst[i] = abs(src[i])
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T, typename U>
__aicore__ inline void Abs(const LocalTensor<T>& dst, const LocalTensor<U>& src, const int32_t& count);
#endif

/* **************************************** Reciprocal ****************************************** */
/*
 * @ingroup Rec Level 0
 * @brief dst[i] = 1/src[i]
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] repeatParams.dstBlkStride dst block stride
 * @param [in] repeatParams.srcBlkStride src block stride
 * @param [in] repeatParams.dstRepStride dst repeat stride
 * @param [in] repeatParams.src0RepStride src repeat stride
 */
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
template <typename T, bool isSetMask = true, const ReciprocalConfig& config = DEFAULT_RECIPROCAL_CONFIG>
__ASC_USE_RESERVED_UBUF__(
    3510, "Reciprocal is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Reciprocal(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);
template <typename T, bool isSetMask = true, const ReciprocalConfig& config = DEFAULT_RECIPROCAL_CONFIG>
__ASC_USE_RESERVED_UBUF__(
    3510, "Reciprocal is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Reciprocal(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);
#else
template <typename T, bool isSetMask = true>
__aicore__ inline void Reciprocal(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);
template <typename T, bool isSetMask = true>
__aicore__ inline void Reciprocal(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);
#endif

/*
 * @ingroup Rec Level 2
 * @brief dst[i] = 1/src[i]
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
template <typename T, const ReciprocalConfig& config = DEFAULT_RECIPROCAL_CONFIG>
__aicore__ inline void Reciprocal(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count);
#else
template <typename T>
__aicore__ inline void Reciprocal(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count);
#endif

/* **************************************** Rsqrt ****************************************** */
/*
 * @ingroup Rsqrt Level 0
 * @brief dst[i] = 1/sqrt(src[i])
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] repeatParams.dstBlkStride dst block stride
 * @param [in] repeatParams.srcBlkStride src block stride
 * @param [in] repeatParams.dstRepStride dst repeat stride
 * @param [in] repeatParams.src0RepStride src repeat stride
 */
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
template <typename T, bool isSetMask = true, const RsqrtConfig& config = DEFAULT_RSQRT_CONFIG>
__ASC_USE_RESERVED_UBUF__(3510, "Rsqrt is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Rsqrt(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);
template <typename T, bool isSetMask = true, const RsqrtConfig& config = DEFAULT_RSQRT_CONFIG>
__ASC_USE_RESERVED_UBUF__(3510, "Rsqrt is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Rsqrt(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);
#else
template <typename T, bool isSetMask = true>
__aicore__ inline void Rsqrt(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);
template <typename T, bool isSetMask = true>
__aicore__ inline void Rsqrt(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);
#endif

/*
 * @ingroup Rsqrt Level 2
 * @brief dst[i] = 1/sqrt(src[i])
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
template <typename T, const RsqrtConfig& config = DEFAULT_RSQRT_CONFIG>
__aicore__ inline void Rsqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count);
#else
template <typename T>
__aicore__ inline void Rsqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count);
#endif

/* **************************************** Sqrt ****************************************** */
/*
 * @ingroup Sqrt Level 0
 * @brief dst[i] = src[i]^(0.5)
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] repeatParams.dstBlkStride dst block stride
 * @param [in] repeatParams.srcBlkStride src block stride
 * @param [in] repeatParams.dstRepStride dst repeat stride
 * @param [in] repeatParams.src0RepStride src repeat stride
 */
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
template <typename T, bool isSetMask = true, const SqrtConfig& config = DEFAULT_SQRT_CONFIG>
__ASC_USE_RESERVED_UBUF__(3510, "Sqrt is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Sqrt(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);
template <typename T, bool isSetMask = true, const SqrtConfig& config = DEFAULT_SQRT_CONFIG>
__ASC_USE_RESERVED_UBUF__(3510, "Sqrt is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Sqrt(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);
#else
template <typename T, bool isSetMask = true>
__aicore__ inline void Sqrt(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);
template <typename T, bool isSetMask = true>
__aicore__ inline void Sqrt(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);
#endif

/*
 * @ingroup Sqrt Level 2
 * @brief dst[i] = src[i]^(0.5)
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
template <typename T, const SqrtConfig& config = DEFAULT_SQRT_CONFIG>
__aicore__ inline void Sqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count);
#else
template <typename T>
__aicore__ inline void Sqrt(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count);
#endif

/* **************************************** Not ****************************************** */
/*
 * @ingroup Not Level 0
 * @brief dst[i] = ~src[i]
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] repeatParams.dstBlkStride dst block stride
 * @param [in] repeatParams.srcBlkStride src block stride
 * @param [in] repeatParams.dstRepStride dst repeat stride
 * @param [in] repeatParams.src0RepStride src repeat stride
 */
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(3510, "Not is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Not(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask[], const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(3510, "Not is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Not(
    const LocalTensor<T>& dst, const LocalTensor<T>& src, uint64_t mask, const uint8_t repeatTime,
    const UnaryRepeatParams& repeatParams);

/*
 * @ingroup Not Level 2
 * @brief dst[i] = ~src[i]
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T>
__aicore__ inline void Not(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count);

/* **************************************** Neg ****************************************** */
/*
 * @ingroup Neg Level 2
 * @brief dst[i] = -src[i]
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T>
__aicore__ inline void Neg(const LocalTensor<T>& dst, const LocalTensor<T>& src, const uint32_t count);
} // namespace AscendC
#pragma end_pipe

#if defined(__NPU_ARCH__)
#include "../../impl/basic_api/kernel_operator_vec_unary_intf_impl.h"
#endif
#endif // ASCENDC_MODULE_OPERATOR_VEC_UNARY_INTERFACE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_UNARY_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_UNARY_INTF_H__
#endif
