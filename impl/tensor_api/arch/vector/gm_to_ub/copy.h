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
#warning                                                                                                               \
    "impl/tensor_api/arch/vector/gm_to_ub/copy.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file copy.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_GM_TO_UB_COPY_H
#define IMPL_TENSOR_API_ARCH_VECTOR_GM_TO_UB_COPY_H

#include "impl/tensor_api/utils/utils_impl.h"
#include "impl/tensor_api/atom/copy_traits_impl.h"
#include "impl/tensor_api/arch/vector/gm_to_ub/routing.h"

namespace asc {
namespace te {

constexpr copy_gm_to_ub_trait DEFAULT_COPY_GM_TO_UB_TRAIT = copy_gm_to_ub_trait{};

struct copy_gm_to_ub_trait_default {
    using trait_type = copy_gm_to_ub_trait;
    static constexpr const trait_type value = DEFAULT_COPY_GM_TO_UB_TRAIT;
};

constexpr copy_gm_to_ub_params DEFAULT_COPY_GM_TO_UB_PARAMS{};

struct copy_gm_to_ub_base {
public:
    template <const copy_gm_to_ub_trait& trait = DEFAULT_COPY_GM_TO_UB_TRAIT, typename T, typename U>
    __aicore__ inline static void data_copy_impl(const T& dst, const U& src,
                                                 const copy_gm_to_ub_params& params = DEFAULT_COPY_GM_TO_UB_PARAMS)
    {
        using dst_pos = get_mem_location<T>;
        using src_pos = get_mem_location<U>;
        static_assert(Std::is_same_v<dst_pos, location::ub>, "When Copy tensor from GM to UB, dst tensor must on UB");
        static_assert(Std::is_same_v<src_pos, location::gm>, "When Copy tensor from GM to UB, src tensor must on GM");
        TENSOR_API_DEBUG_CHECK(debug_check_layout, dst.layout(), "dst", "copy_gm_to_ub");
        TENSOR_API_DEBUG_CHECK(debug_check_layout, src.layout(), "src", "copy_gm_to_ub");
        TENSOR_API_DEBUG_CHECK(debug_check_copy_size, src, dst, "copy_gm_to_ub");
        using dst_layout_ptn = get_layout_pattern<typename T::layout_type>;
        using src_layout_ptn = get_layout_pattern<typename U::layout_type>;
        using copy_gm_to_ub_impl = typename copy_gm_to_ub_routing<CURRENT_ARCH_VERSION, dst_layout_ptn, src_layout_ptn>::type;
        copy_gm_to_ub_impl::template run<trait, T, U>(dst, src, params);
    }
};

struct copy_gm_to_ub : public copy_gm_to_ub_base {
public:
    template <typename Tp, const Tp& traits, typename... Args>
    __aicore__ inline static void copy(const Args&... args)
    {
        if ASCEND_IS_AIV {
            data_copy_impl<traits>(args...);
        }
    }
};

struct copy_gm_to_ub_with : public copy_gm_to_ub_base {
public:
    template <typename Tp, const Tp& traits, typename... Args>
    __aicore__ inline static void copy(const Args&... args)
    {
        if ASCEND_IS_AIV {
            data_copy_impl<traits>(args...);
        }
    }
};

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ARCH_VECTOR_GM_TO_UB_COPY_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
