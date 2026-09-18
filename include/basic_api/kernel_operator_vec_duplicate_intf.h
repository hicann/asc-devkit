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
 * \file kernel_operator_vec_duplicate_intf.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "kernel_operator_vec_duplicate_intf.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_DUPLICATE_INTF_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_VEC_DUPLICATE_INTERFACE_H
#define ASCENDC_MODULE_OPERATOR_VEC_DUPLICATE_INTERFACE_H

#include "../../impl/basic_api/kernel_macros.h"
#include "kernel_tensor.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstdint>
#include "stub_def.h"
#endif

#pragma begin_pipe(V)
namespace AscendC {
/* **************************************************************************************************
 * Duplicate                                            *
 * ************************************************************************************************* */
/*
 * @ingroup Duplicate Level 0
 * @brief dst[i] = scalar
 * @param [out] dst output LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] mask[]/mask mask array/count
 * @param [in] repeatTime repeat times
 * @param [in] dstBlockStride dst block stride
 * @param [in] dstRepeatStride dst repeat stride
 */
template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(3510, "Duplicate is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Duplicate(
    const LocalTensor<T>& dst, const T& scalarValue, uint64_t mask, const uint8_t repeatTime,
    const uint16_t dstBlockStride, const uint8_t dstRepeatStride);

template <typename T, bool isSetMask = true>
__ASC_USE_RESERVED_UBUF__(3510, "Duplicate is forbidden when compile option --cce-disable-asc-reserved-ubuf is enabled")
__aicore__ inline void Duplicate(
    const LocalTensor<T>& dst, const T& scalarValue, uint64_t mask[], const uint8_t repeatTime,
    const uint16_t dstBlockStride, const uint8_t dstRepeatStride);

/*
 * @ingroup Duplicate Level 2
 * @brief dst = dst[i] = scalar
 * @param [out] dst output LocalTensor
 * @param [in] scalar input scalar number
 * @param [in] count number Number of data involved in calculation
 */
template <typename T>
__aicore__ inline void Duplicate(const LocalTensor<T>& dst, const T& scalarValue, const int32_t& count);

#if (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162)
/*
 * @ingroup Duplicate lowest position of src
 * @brief dst = dst[i] = src[0]
 * @param [out] dst output LocalTensor
 * @param [in] src input LocalTensor
 * @param [in] count number Number of data involved in calculation
 */
template <typename T>
__aicore__ inline void Duplicate(const LocalTensor<T>& dst, const LocalTensor<T>& src, const int32_t& count);
#endif

/* **************************************************************************************************
 * Interleave                                            *
 * ************************************************************************************************* */
/*
 * @ingroup Interleave Level 2
 * @brief Interleave src0 and src1 to dst0 and dst1
 * @param [out] dst0 output0 LocalTensor
 * @param [out] dst1 output1 LocalTensor
 * @param [in] src0 input0 LocalTensor
 * @param [in] src1 input1 LocalTensor
 * @param [count] count number of data calculation, must be even number
 */
template <typename T>
__aicore__ inline void Interleave(
    const LocalTensor<T>& dst0, const LocalTensor<T>& dst1, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
    const int32_t count);

/* **************************************************************************************************
 * DeInterleave                                            *
 * ************************************************************************************************* */
/*
 * @ingroup DeInterleave Level 2
 * @brief DeInterleave src0 and src1 to dst0 and dst1
 * @param [out] dst0 output0 LocalTensor
 * @param [out] dst1 output1 LocalTensor
 * @param [in] src0 input0 LocalTensor
 * @param [in] src1 input1 LocalTensor
 * @param [count] count number of data calculation, must be even number
 */
template <typename T>
__aicore__ inline void DeInterleave(
    const LocalTensor<T>& dst0, const LocalTensor<T>& dst1, const LocalTensor<T>& src0, const LocalTensor<T>& src1,
    const int32_t count);

/*
 * @ingroup DeInterleave Level 2
 * @brief DeInterleave src to dst0 and dst1
 * @param [out] dst0 output0 LocalTensor
 * @param [out] dst1 output1 LocalTensor
 * @param [in] src input LocalTensor
 * @param [srcCount] srcCount number of data calculation, must be even number
 */
template <typename T>
__aicore__ inline void DeInterleave(
    const LocalTensor<T>& dst0, const LocalTensor<T>& dst1, const LocalTensor<T>& src, const int32_t srcCount);
} // namespace AscendC
#pragma end_pipe
#if defined(__NPU_ARCH__)
#include "../../impl/basic_api/kernel_operator_vec_duplicate_intf_impl.h"
#endif
#endif // ASCENDC_MODULE_OPERATOR_VEC_DUPLICATE_INTERFACE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_DUPLICATE_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_VEC_DUPLICATE_INTF_H__
#endif
