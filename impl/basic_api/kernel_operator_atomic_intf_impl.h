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
 * \file kernel_operator_atomic_intf_impl.h
 * \brief
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/kernel_operator_atomic_intf_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_atomic_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_ATOMIC_INTF_IMPL_H__
#endif
#ifndef ASCENDC_MODULE_OPERATOR_ATOMIC_INTERFACE_IMPL_H
#define ASCENDC_MODULE_OPERATOR_ATOMIC_INTERFACE_IMPL_H
#include "../../include/basic_api/kernel_tensor.h"

#if defined(__NPU_ARCH__) && ((__NPU_ARCH__ == 5102) || (__NPU_ARCH__ == 5162) || (__NPU_ARCH__ == 3510))
#if (__NPU_ARCH__ == 5102)
#include "dav_m510/kernel_operator_atomic_impl.h"
#elif (__NPU_ARCH__ == 5162)
#include "dav_v516/kernel_operator_atomic_impl.h"
#else
#include "dav_3510/kernel_operator_atomic_impl.h"
#endif

namespace AscendC {
template <typename T>
__aicore__ inline T AtomicAdd(__gm__ T* address, T value)
{
    return AtomicAddImpl(address, value);
}

template <typename T>
__aicore__ inline T AtomicMax(__gm__ T* address, T value)
{
    return AtomicMaxImpl(address, value);
}

template <typename T>
__aicore__ inline T AtomicMin(__gm__ T* address, T value)
{
    return AtomicMinImpl(address, value);
}

template <typename T>
__aicore__ inline T AtomicExch(__gm__ T* address, T value)
{
    return AtomicExchImpl(address, value);
}

template <typename T>
__aicore__ inline T AtomicCas(__gm__ T* address, T value1, T value2)
{
    return AtomicCasImpl(address, value1, value2);
}
} // namespace AscendC
#endif
#endif // ASCENDC_MODULE_OPERATOR_ATOMIC_INTERFACE_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_ATOMIC_INTF_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_ATOMIC_INTF_IMPL_H__
#endif
