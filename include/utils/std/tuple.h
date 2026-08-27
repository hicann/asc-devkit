/**
* Copyright (c) 2025 Huawei Technologies Co., Ltd.
* This program is free software, you can redistribute it and/or modify it under the terms and conditions of
* CANN Open Software License Agreement Version 2.0 (the "License").
* Please refer to the License for details. You may not use this file except in compliance with the License.
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
* INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
* See LICENSE in the root of the software repository for the full text of the License.
*/

/* !
 * \file tuple.h
 * \brief
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "tuple.h cannot be used with compile flag --enable-simt enabled."
#endif

#ifndef AICORE_UTILS_STD_TUPLE_H
#define AICORE_UTILS_STD_TUPLE_H

#if defined(ASCENDC_CPU_DEBUG) && ASCENDC_CPU_DEBUG == 1
#include "stub_def.h"
#endif

#include "impl/utils/std/tuple/tuple_impl.h"

namespace AscendC {
namespace Std {

// tuple
template <typename ...Tps>
class tuple;

// tuple_size
template <typename ...Tps>
struct tuple_size;

// tuple_element
template <size_t N, typename ...Tps>
struct tuple_element;

// make_tuple
template <typename ...Tps>
ASCENDC_HOST_AICORE inline constexpr tuple<unwrap_decay_t<Tps>...> make_tuple(Tps&& ...args);

// tie
template <typename ...Tps>
ASCENDC_HOST_AICORE inline constexpr tuple<Tps& ...> tie(Tps& ...args) noexcept;

// get
template <size_t N, typename ...Tps>
ASCENDC_HOST_AICORE inline constexpr typename tuple_element<N, tuple<Tps...> >::type& get(tuple<Tps...>& t) noexcept;

template <size_t N, typename ...Tps>
ASCENDC_HOST_AICORE inline constexpr const typename tuple_element<N, tuple<Tps...> >::type& get(const tuple<Tps...>& t) noexcept;

template <size_t N, typename ...Tps>
ASCENDC_HOST_AICORE inline constexpr typename tuple_element<N, tuple<Tps...> >::type&& get(tuple<Tps...>&& t) noexcept;

template <size_t N, typename ...Tps>
ASCENDC_HOST_AICORE inline constexpr const typename tuple_element<N, tuple<Tps...> >::type&& get(const tuple<Tps...>&& t) noexcept;

}
}

#endif // AICORE_UTILS_STD_TUPLE_H
