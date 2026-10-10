/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */
#if !defined(__ASCENDC_INCLUDE_INTERNAL_HEADERS__)
#pragma message( \
    "impl/basic_api/dav_v516/kernel_operator_atomic_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_ATOMIC_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_ATOMIC_IMPL_H
#define ASCENDC_MODULE_OPERATOR_ATOMIC_IMPL_H

namespace AscendC {
// atomic_add
template <typename T>
__aicore__ inline T AtomicAddImpl(__gm__ T* address, T value)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "AtomicAdd is not supported on current device"); });
}

// atomic_max
template <typename T>
__aicore__ inline T AtomicMaxImpl(__gm__ T* address, T value)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "AtomicMax is not supported on current device"); });
}

// atomic_min
template <typename T>
__aicore__ inline T AtomicMinImpl(__gm__ T* address, T value)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "AtomicMin is not supported on current device"); });
}

// atomic_cas
template <typename T>
__aicore__ inline T AtomicCasImpl(__gm__ T* address, T value1, T value2)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "AtomicCas is not supported on current device"); });
}

// atomic_exch
template <typename T>
__aicore__ inline T AtomicExchImpl(__gm__ T* address, T value)
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "AtomicExch is not supported on current device"); });
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_ATOMIC_ADD_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_ATOMIC_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_ATOMIC_IMPL_H__
#endif
