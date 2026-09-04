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
 * \file kernel_operator_scalar_intf.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "kernel_operator_scalar_intf.h cannot be used with compile flag --enable-simt enabled."
#endif

#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SCALAR_INTF_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_SCALAR_INTERFACE_H
#define ASCENDC_MODULE_OPERATOR_SCALAR_INTERFACE_H

#include "../../impl/basic_api/kernel_macros.h"
#include "../../impl/basic_api/utils/kernel_utils_mode.h"

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include <cstdint>
#include "stub_def.h"
#endif

namespace AscendC {
template <int countValue>
__aicore__ inline int64_t GetBitCount(uint64_t valueIn);

// ScalarGetCountOfValue has been updated, please use GetBitCount instead.
template <int countValue>
__aicore__ inline int64_t ScalarGetCountOfValue(uint64_t valueIn);

__aicore__ inline int64_t CountLeadingZero(uint64_t valueIn);

// ScalarCountLeadingZero has been updated, please use CountLeadingZero instead.
__aicore__ inline int64_t ScalarCountLeadingZero(uint64_t valueIn);

template <typename T>
__aicore__ inline void GetUintDivMagicAndShift(T& magic, T& shift, T divisor);

__aicore__ inline int64_t CountBitsCntSameAsSignBit(int64_t valueIn);

template <int countValue>
__aicore__ inline int64_t GetSFFValue(uint64_t valueIn);

// ScalarGetSFFValue has been updated, please use GetSFFValue instead.
template <int countValue>
__aicore__ inline int64_t ScalarGetSFFValue(uint64_t valueIn);

template <typename T, typename U, RoundMode roundMode>
__aicore__ inline U Cast(T valueIn);

// ScalarCast has been updated, please use Cast instead.
template <typename T, typename U, RoundMode roundMode>
__aicore__ inline U ScalarCast(T valueIn);

#if defined(__NPU_ARCH__) &&                                                                                 \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102) || \
     (__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || (__NPU_ARCH__ == 5101) || (__NPU_ARCH__ == 5161) || \
     (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
// 所有架构共用的：float -> bfloat16_t
__aicore__ inline bfloat16_t Cast(const float& fVal);
#endif

#if defined(__NPU_ARCH__) && \
    ((__NPU_ARCH__ == 2201) || (__NPU_ARCH__ == 3002) || (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
// 各架构特化的：类型 -> float
#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
template <typename T, typename U, typename>
__aicore__ constexpr inline U Cast(T bVal);
#else
__aicore__ inline float Cast(const bfloat16_t& bVal);
#endif

#elif defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3003) || (__NPU_ARCH__ == 3113) || (__NPU_ARCH__ == 5101) || \
                                (__NPU_ARCH__ == 5161) || (__NPU_ARCH__ == 5165) || (__NPU_ARCH__ == 5163))
template <typename T>
__aicore__ constexpr inline float Cast(const T& bVal);
#endif

template <typename T>
[[deprecated("Deprecated since 9.2.0, will be removed after 2027/12/30, Use WriteGmBypassDCache instead.")]]
__aicore__ inline void WriteGmByPassDCache(__gm__ T* addr, T value);

template <typename T>
[[deprecated("Deprecated since 9.2.0, will be removed after 2027/12/30, Use ReadGmBypassDCache instead.")]]
__aicore__ inline T ReadGmByPassDCache(__gm__ T* addr);

template <typename T>
__aicore__ inline void WriteGmBypassDCache(__gm__ T* addr, T value);

template <typename T>
__aicore__ inline T ReadGmBypassDCache(__gm__ T* addr);
} // namespace AscendC

#if defined(__NPU_ARCH__)
#include "../../impl/basic_api/kernel_operator_scalar_intf_impl.h"
#endif
#endif // ASCENDC_MODULE_OPERATOR_SCALAR_INTERFACE_H

#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SCALAR_INTF_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SCALAR_INTF_H__
#endif
