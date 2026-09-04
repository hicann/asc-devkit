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
 * \file kernel_operator_scalar_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_scalar_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_scalar_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SCALAR_INTF_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_SCALAR_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_SCALAR_INTERFACE_IMPL_H
#include "../../include/basic_api/kernel_tensor.h"
#include "kernel_scalar.h"
#include "kernel_scalar_convert.h"

namespace AscendC {
template <int countValue>
__aicore__ inline int64_t GetBitCount(uint64_t valueIn)
{
    return GetBitCountImpl<countValue>(valueIn);
}

// ScalarGetCountOfValue has been updated, please use GetBitCount instead.
template <int countValue>
__aicore__ inline int64_t ScalarGetCountOfValue(uint64_t valueIn)
{
    return GetBitCount<countValue>(valueIn);
}

__aicore__ inline int64_t CountLeadingZero(uint64_t valueIn) { return CountLeadingZeroImpl(valueIn); }

// ScalarCountLeadingZero has been updated, please use CountLeadingZero instead.
__aicore__ inline int64_t ScalarCountLeadingZero(uint64_t valueIn) { return CountLeadingZero(valueIn); }

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102))
template <typename T>
__aicore__ inline void GetUintDivMagicAndShift(T& magic, T& shift, T divisor)
{
    return GetUintDivMagicAndShiftImpl(magic, shift, divisor);
}
#endif

__aicore__ inline int64_t CountBitsCntSameAsSignBit(int64_t valueIn) { return CountBitsCntSameAsSignBitImpl(valueIn); }

template <int countValue>
__aicore__ inline int64_t GetSFFValue(uint64_t valueIn)
{
    return GetSFFValueImpl<countValue>(valueIn);
}

// ScalarGetSFFValue has been updated, please use GetSFFValue instead.
template <int countValue>
__aicore__ inline int64_t ScalarGetSFFValue(uint64_t valueIn)
{
    return GetSFFValue<countValue>(valueIn);
}

template <typename T, typename U, RoundMode roundMode>
__aicore__ inline U Cast(T valueIn)
{
    return CastImpl<T, U, roundMode>(valueIn);
}

// ScalarCast has been updated, please use Cast instead.
template <typename T, typename U, RoundMode roundMode>
__aicore__ inline U ScalarCast(T valueIn)
{
    return ScalarCastImpl<T, U, roundMode>(valueIn);
}

#if __NPU_ARCH__ == 2201 || (__NPU_ARCH__ == 3510) || (__NPU_ARCH__ == 5102)
template <typename T>
[[deprecated("Deprecated since 9.2.0, will be removed after 2027/12/30, Use WriteGmBypassDCache instead.")]]
__aicore__ inline void WriteGmByPassDCache(__gm__ T* addr, T value)
{
    return WriteGmBypassDCacheImpl(addr, value);
}

template <typename T>
[[deprecated("Deprecated since 9.2.0, will be removed after 2027/12/30, Use ReadGmBypassDCache instead.")]]
__aicore__ inline T ReadGmByPassDCache(__gm__ T* addr)
{
    return ReadGmBypassDCacheImpl(addr);
}

template <typename T>
__aicore__ inline void WriteGmBypassDCache(__gm__ T* addr, T value)
{
    return WriteGmBypassDCacheImpl(addr, value);
}

template <typename T>
__aicore__ inline T ReadGmBypassDCache(__gm__ T* addr)
{
    return ReadGmBypassDCacheImpl(addr);
}
#endif
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_SCALAR_INTERFACE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SCALAR_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SCALAR_INTF_IMPL_H__
#endif
