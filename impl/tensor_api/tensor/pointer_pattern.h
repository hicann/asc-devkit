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
    "impl/tensor_api/tensor/pointer_pattern.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file pointer_pattern.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_TENSOR_POINTER_PATTERN_H
#define IMPL_TENSOR_API_TENSOR_POINTER_PATTERN_H

#include "impl/tensor_api/utils/utils_impl.h"
#include "impl/tensor_api/tensor/pointer_mem_impl.h"

namespace AscendC {
namespace Te {

template <typename T = uint16_t>
struct PtrTrait {
    using type = T;
};

template <typename T, typename = void>
struct IsPtrTrait : Std::false_type {};

template <typename T>
struct IsPtrTrait<T, void_t<typename T::type>> : Std::true_type {};

template <typename T>
using MemPtrTraitT = typename Std::conditional<IsPtrTrait<T>::value, T, PtrTrait<T>>::type;

template <typename Hardware, typename Arg>
using EnableMakePtrByTrait =
    Std::enable_if_t<IsHardwareV<Hardware> && !IsMemPtrIterator<Std::remove_cvref_t<Arg>>::value, int>;

template <typename Hardware, typename Arg>
using EnableMakeHardwarePtr =
    Std::enable_if_t<IsHardwareV<Hardware> && IsMemPtrIterator<Std::remove_cvref_t<Arg>>::value, int>;

template <typename Iterator>
using EnableMakePtrByIter = Std::enable_if_t<IsMemPtrIterator<Std::remove_cvref_t<Iterator>>::value, int>;

template <typename PtrPattern, typename DataType, typename Addr, EnableMakePtrByTrait<PtrPattern, Addr> = 0>
__aicore__ inline auto MakeMemPtr(Addr arg)
{
    using pointer = typename locationAttr<DataType>::locationMap::template Get<PtrPattern>;
    return MakeLocationMemPtr<PtrPattern>(reinterpret_cast<pointer>(asc_get_phy_buf_addr(0) + arg));
}

template <typename PtrPattern, typename Iterator, EnableMakeHardwarePtr<PtrPattern, Iterator> = 0>
__aicore__ inline constexpr auto MakeMemPtr(Iterator iterator)
{
    return MakeLocationMemPtr<PtrPattern>(iterator);
}

template <typename Iterator, EnableMakePtrByIter<Iterator> = 0>
__aicore__ inline constexpr auto MakeMemPtr(Iterator iterator)
{
    using PtrPattern = GetAttributeLocation<typename IterEle<Iterator>::type*>;
    return MakeLocationMemPtr<PtrPattern>(iterator);
}

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_TENSOR_POINTER_PATTERN_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
