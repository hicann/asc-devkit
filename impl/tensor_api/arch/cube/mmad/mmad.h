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
    "impl/tensor_api/arch/cube/mmad/mmad.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file mmad.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_MMAD_MMAD_H
#define IMPL_TENSOR_API_ARCH_CUBE_MMAD_MMAD_H

#include "impl/tensor_api/arch/cube/mmad/routing.h"

namespace asc {
namespace te {

constexpr mmad_trait DEFAULT_MMAD_TRAIT;

constexpr mmad_params default_mmad_params = {0, 0, 0, 0, true};

constexpr mmad_params default_mmad_with_bias_params = {0, 0, 0, 0, false};

struct mmad_trait_default {
    using trait_type = mmad_trait;
    static constexpr const trait_type value = DEFAULT_MMAD_TRAIT;
};

struct mmad_operation {
public:
    template <typename Tp, const Tp& traits, typename... Args>
    __aicore__ inline static void mmad(const Args&... args)
    {
        if ASCEND_IS_AIC {
            mmad_impl<traits, Args...>(args...);
        }
    }

private:
    template <const mmad_trait& trait = DEFAULT_MMAD_TRAIT, typename T, typename U, typename S, typename Params>
    __aicore__ inline static void mmad_impl(const T& dst, const U& fm, const S& filter, const Params& params)
    {
        using dst_pos = get_mem_location<T>;
        using fm_pos = get_mem_location<U>;
        using filter_pos = get_mem_location<S>;
        static_assert(Std::is_same_v<dst_pos, location::l0c>, "When Mmad, dst tensor must be from L0C.");
        static_assert(Std::is_same_v<fm_pos, location::l0a>, "When Mmad, fm tensor must be from L0A.");
        static_assert(Std::is_same_v<filter_pos, location::l0b>, "When Mmad, filter tensor must be from L0B.");
        using dst_layout = typename T::layout_type;
        using fm_layout = typename U::layout_type;
        using filter_layout = typename S::layout_type;
        using dst_layout_ptn = get_layout_pattern<dst_layout>;
        using fm_layout_ptn = get_layout_pattern<fm_layout>;
        using filter_layout_ptn = get_layout_pattern<filter_layout>;
        using mmad_impl = typename mmad_routing<CURRENT_ARCH_VERSION, dst_layout_ptn, fm_layout_ptn, filter_layout_ptn,
                                               location::invalid>::type;
        mmad_impl::template run<trait>(dst, fm, filter, params);
    }

    template <const mmad_trait& trait = DEFAULT_MMAD_TRAIT, typename T, typename U, typename S, typename V,
              typename Params>
    __aicore__ inline static void mmad_impl(const T& dst, const U& fm, const S& filter, const V& bias,
                                            const Params& params)
    {
        using dst_pos = get_mem_location<T>;
        using fm_pos = get_mem_location<U>;
        using filter_pos = get_mem_location<S>;
        using bias_pos = get_mem_location<V>;
        static_assert(Std::is_same_v<dst_pos, location::l0c>, "When Mmad, dst tensor must be from L0C.");
        static_assert(Std::is_same_v<fm_pos, location::l0a>, "When Mmad, fm tensor must be from L0A.");
        static_assert(Std::is_same_v<filter_pos, location::l0b>, "When Mmad, filter tensor must be from L0B.");
        static_assert(Std::is_same_v<bias_pos, location::l0c> || Std::is_same_v<bias_pos, location::bias>,
                      "When Mmad, bias tensor must be from L0C or BIAS.");
        using dst_layout = typename T::layout_type;
        using fm_layout = typename U::layout_type;
        using filter_layout = typename S::layout_type;
        using dst_layout_ptn = get_layout_pattern<dst_layout>;
        using fm_layout_ptn = get_layout_pattern<fm_layout>;
        using filter_layout_ptn = get_layout_pattern<filter_layout>;
        using mmad_impl =
            typename mmad_routing<CURRENT_ARCH_VERSION, dst_layout_ptn, fm_layout_ptn, filter_layout_ptn, bias_pos>::type;
        mmad_impl::template run<trait>(dst, fm, filter, bias, params);
    }
};

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ARCH_CUBE_MMAD_MMAD_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
