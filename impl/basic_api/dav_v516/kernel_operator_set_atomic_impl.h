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
    "impl/basic_api/dav_v516/kernel_operator_set_atomic_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file may be removed in the future. Please use \"#include \"basic_api/kernel_operator_intf.h\"\" and use public functions or variables defined in interface headers files.")
#define __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#define __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SET_ATOMIC_IMPL_H__
#endif

#ifndef ASCENDC_MODULE_OPERATOR_SET_ATOMIC_ADD_IMPL_H
#define ASCENDC_MODULE_OPERATOR_SET_ATOMIC_ADD_IMPL_H

namespace AscendC {
// set_atomic_none
__aicore__ inline void SetAtomicNoneImpl() { set_atomic_none(); }
// set_atomic_add
template <typename T>
__aicore__ inline void SetAtomicAddImpl()
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetAtomicAdd is not supported on current device"); });
}

template <>
__aicore__ inline void SetAtomicAddImpl<float>()
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetAtomicAdd is not supported on current device"); });
}

template <>
__aicore__ inline void SetAtomicAddImpl<half>()
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetAtomicAdd is not supported on current device"); });
}

template <>
__aicore__ inline void SetAtomicAddImpl<int16_t>()
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetAtomicAdd is not supported on current device"); });
}

template <>
__aicore__ inline void SetAtomicAddImpl<int32_t>()
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetAtomicAdd is not supported on current device"); });
}

template <>
__aicore__ inline void SetAtomicAddImpl<int8_t>()
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetAtomicAdd is not supported on current device"); });
}

template <>
__aicore__ inline void SetAtomicAddImpl<bfloat16_t>()
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetAtomicAdd is not supported on current device"); });
}
// set_atomic_max
template <typename T>
__aicore__ inline void SetAtomicMaxImpl()
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetAtomicMax is not supported on current device"); });
}

template <>
__aicore__ inline void SetAtomicMaxImpl<float>()
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetAtomicMax is not supported on current device"); });
}

template <>
__aicore__ inline void SetAtomicMaxImpl<half>()
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetAtomicMax is not supported on current device"); });
}

template <>
__aicore__ inline void SetAtomicMaxImpl<int16_t>()
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetAtomicMax is not supported on current device"); });
}

template <>
__aicore__ inline void SetAtomicMaxImpl<int32_t>()
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetAtomicMax is not supported on current device"); });
}

template <>
__aicore__ inline void SetAtomicMaxImpl<int8_t>()
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetAtomicMax is not supported on current device"); });
}

template <>
__aicore__ inline void SetAtomicMaxImpl<bfloat16_t>()
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetAtomicMax is not supported on current device"); });
}

// set_atomic_min
template <typename T>
__aicore__ inline void SetAtomicMinImpl()
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetAtomicMin is not supported on current device"); });
}

template <>
__aicore__ inline void SetAtomicMinImpl<float>()
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetAtomicMin is not supported on current device"); });
}

template <>
__aicore__ inline void SetAtomicMinImpl<half>()
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetAtomicMin is not supported on current device"); });
}

template <>
__aicore__ inline void SetAtomicMinImpl<int16_t>()
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetAtomicMin is not supported on current device"); });
}

template <>
__aicore__ inline void SetAtomicMinImpl<int32_t>()
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetAtomicMin is not supported on current device"); });
}

template <>
__aicore__ inline void SetAtomicMinImpl<int8_t>()
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetAtomicMin is not supported on current device"); });
}

template <>
__aicore__ inline void SetAtomicMinImpl<bfloat16_t>()
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetAtomicMin is not supported on current device"); });
}

template <typename T>
__aicore__ inline void SetAtomicTypeImpl()
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetAtomicType is not supported on current device"); });
}

template <>
__aicore__ inline void SetAtomicTypeImpl<float>()
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetAtomicType is not supported on current device"); });
}

template <>
__aicore__ inline void SetAtomicTypeImpl<half>()
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetAtomicType is not supported on current device"); });
}

template <>
__aicore__ inline void SetAtomicTypeImpl<int16_t>()
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetAtomicType is not supported on current device"); });
}

template <>
__aicore__ inline void SetAtomicTypeImpl<int32_t>()
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetAtomicType is not supported on current device"); });
}

template <>
__aicore__ inline void SetAtomicTypeImpl<int8_t>()
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetAtomicType is not supported on current device"); });
}

template <>
__aicore__ inline void SetAtomicTypeImpl<bfloat16_t>()
{
    ASCENDC_ASSERT(false, { KERNEL_LOG(KERNEL_ERROR, "SetAtomicType is not supported on current device"); });
}
} // namespace AscendC
#endif // ASCENDC_MODULE_OPERATOR_SET_ATOMIC_ADD_IMPL_H
#if defined(__UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SET_ATOMIC_IMPL_H__)
#undef __ASCENDC_INCLUDE_INTERNAL_HEADERS__
#undef __UNDEF_ASCENDC_INCLUDE_INTERNAL_HEADERS_KERNEL_OPERATOR_SET_ATOMIC_IMPL_H__
#endif
