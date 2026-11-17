/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#warning \\
    "impl/tensor_api/arch/scalar_compute/scalar_load_store_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use \\"#include \\"tensor_api/arch/scalar_compute/scalar_load_store.h\\" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_LOAD_STORE_IMPL_H
#endif

#ifndef IMPL_TENSOR_API_ARCH_SCALAR_COMPUTE_SCALAR_LOAD_STORE_IMPL_H
#define IMPL_TENSOR_API_ARCH_SCALAR_COMPUTE_SCALAR_LOAD_STORE_IMPL_H

#include "c_api/asc_simd.h"
#include "tensor_api/utils/utils.h"

namespace asc {
namespace te {

namespace detail {
template <typename T>
constexpr bool is_scalar_dev_type_v =
    AscendC::Std::is_same_v<T, int8_t> || AscendC::Std::is_same_v<T, uint8_t> || AscendC::Std::is_same_v<T, int16_t> ||
    AscendC::Std::is_same_v<T, uint16_t> || AscendC::Std::is_same_v<T, int32_t> ||
    AscendC::Std::is_same_v<T, uint32_t> || AscendC::Std::is_same_v<T, int64_t> || AscendC::Std::is_same_v<T, uint64_t>;
} // namespace detail

template <typename T>
__aicore__ inline T load_dev(__gm__ T* addr)
{
    static_assert(detail::is_scalar_dev_type_v<T>, "load_dev only supports 8/16/32/64-bit integer types");
    return asc_load_dev(addr);
}

template <typename T>
__aicore__ inline void store_dev(__gm__ T* addr, T value)
{
    static_assert(detail::is_scalar_dev_type_v<T>, "store_dev only supports 8/16/32/64-bit integer types");
    asc_store_dev(addr, value);
}

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ARCH_SCALAR_COMPUTE_SCALAR_LOAD_STORE_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_LOAD_STORE_IMPL_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_LOAD_STORE_IMPL_H
#endif
