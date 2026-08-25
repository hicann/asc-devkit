/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning \
    "impl/tensor_api/tensor/pointer_mem_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file pointer_mem_impl.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_TENSOR_POINTER_MEM_IMPL_H
#define IMPL_TENSOR_API_TENSOR_POINTER_MEM_IMPL_H

#include "impl/tensor_api/tensor/pointer_adaptor_impl.h"
namespace AscendC {
namespace Te {

template <typename PtrPattern, typename Pointer>
struct HardwareMemPtr : IterAdaptor<Pointer, HardwareMemPtr<PtrPattern, Pointer>> {
    using IterAdaptor<Pointer, HardwareMemPtr<PtrPattern, Pointer>>::IterAdaptor;
    using ptrPattern = PtrPattern;
};

template <typename T>
using GetMemLocation = typename T::iterator::ptrPattern;

// is hardware mem
template <typename PtrPattern, typename Pointer, typename = void>
struct IsHardwareMem : Std::false_type {};

template <typename PtrPattern, typename Pointer>
struct IsHardwareMem<PtrPattern, HardwareMemPtr<PtrPattern, Pointer>> : Std::true_type {};

template <typename PtrPattern, typename Pointer>
struct IsHardwareMem<PtrPattern, Pointer, void_t<typename Pointer::iterator>>
    : IsHardwareMem<PtrPattern, typename Pointer::iterator> {};

template <typename PtrPattern, typename Pointer>
constexpr bool IsHardwareMemV = IsHardwareMem<PtrPattern, Pointer>::value;

template <typename T>
struct IsHardwareMemPtr : Std::false_type {};

template <typename PtrPattern, typename Pointer>
struct IsHardwareMemPtr<HardwareMemPtr<PtrPattern, Pointer>> : Std::true_type {};

template <typename T>
constexpr bool IsHardwareMemPtrV = IsHardwareMemPtr<Std::remove_cvref_t<T>>::value;

template <typename T, typename = void>
struct IsMemPtrIterator : Std::false_type {};

template <typename T>
struct IsMemPtrIterator<T, void_t<decltype(*Std::declval<T&>())>> : Std::true_type {};

template <typename PtrPattern, typename Iterator>
__aicore__ inline auto MakeLocationMemPtr(Iterator iter)
{
    return HardwareMemPtr<PtrPattern, Iterator>{iter};
}
} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_TENSOR_POINTER_MEM_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
