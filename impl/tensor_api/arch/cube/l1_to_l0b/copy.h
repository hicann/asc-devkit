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
    "impl/tensor_api/arch/cube/l1_to_l0b/copy.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file copy.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0B_COPY_H
#define IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0B_COPY_H

#include "impl/tensor_api/arch/cube/l1_to_l0b/routing.h"

namespace asc {
namespace te {

constexpr copy_l1_to_l0b_trait DEFAULT_COPY_L1_TO_L0B_TRAIT;

struct copy_l1_to_l0b_trait_default {
    using trait_type = copy_l1_to_l0b_trait;
    static constexpr const trait_type value = DEFAULT_COPY_L1_TO_L0B_TRAIT;
};

using copy_l1_to_l0b_mode_set =
    tuple_map<Std::tuple<Std::tuple<_1, _0>, copy_mode::normal>, Std::tuple<Std::tuple<_1, _1>, copy_mode::normal>,
             Std::tuple<Std::tuple<_0, _0>, copy_mode::trans>, Std::tuple<Std::tuple<_0, _1>, copy_mode::trans_b8b4>>;

struct copy_l1_to_l0b {
public:
    template <typename Tp, const Tp& traits, typename... Args>
    __aicore__ inline static void copy(const Args&... args)
    {
        if ASCEND_IS_AIC {
            load_data<traits, Args...>(args...);
        }
    }

private:
    template <const copy_l1_to_l0b_trait& trait = DEFAULT_COPY_L1_TO_L0B_TRAIT, typename T, typename U>
    __aicore__ inline static void load_data(const T& dst, const U& src)
    {
        using dst_pos = get_mem_location<T>;
        using src_pos = get_mem_location<U>;
        static_assert(Std::is_same_v<dst_pos, location::l0b>,
                      "When Copy tensor from L1 to L0B, dst tensor must be from L0B.");
        static_assert(Std::is_same_v<src_pos, location::l1>,
                      "When Copy tensor from L1 to L0B, src tensor must be from L1.");
        using dst_layout = typename T::layout_type;
        using src_layout = typename U::layout_type;
        using dst_pattern = get_layout_pattern<dst_layout>;
        using src_pattern = get_layout_pattern<src_layout>;
        TENSOR_API_DEBUG_CHECK(debug_check_layout, dst.layout(), "dst", "copy_l1_to_l0b");
        TENSOR_API_DEBUG_CHECK(debug_check_layout, src.layout(), "src", "copy_l1_to_l0b");
        TENSOR_API_DEBUG_CHECK(debug_check_copy_size, src, dst, "copy_l1_to_l0b");
        if constexpr (dst_layout::depth == FIVE_DIM_DATA && src_layout::depth == FIVE_DIM_DATA) {
            TENSOR_API_DEBUG_CHECK(debug_check_batch_match, get<0>(src.layout().shape()), get<0>(dst.layout().shape()),
                                   "copy_l1_to_l0b");
            TENSOR_API_DEBUG_CHECK(debug_check_l0_batch_stride, get<0>(src.layout().stride()),
                                   remove_batch_dim(src.layout()).capacity(), get<0>(dst.layout().stride()),
                                   remove_batch_dim(dst.layout()).capacity(), "copy_l1_to_l0b");
        }
        constexpr auto is_b8_b4_type = sizeof(typename T::element_type) == 1;
        constexpr auto no_trans = Std::is_same_v<dst_pattern, src_pattern>;
        using copy_l1_to_l0b_mode =
            typename copy_l1_to_l0b_mode_set::template get<Std::tuple<Std::Int<no_trans>, Std::Int<is_b8_b4_type>>>;
        static_assert(!Std::is_same_v<copy_l1_to_l0b_mode, Std::ignore_t>, "Unsupported CopyL12L0B mode.");
        using copy_l1_to_l0b_impl =
            typename copy_l1_to_l0b_routing<CURRENT_ARCH_VERSION, dst_pattern, src_pattern, copy_l1_to_l0b_mode>::type;
        copy_l1_to_l0b_impl::template run<trait, T, U>(dst, src);
    }
};

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0B_COPY_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
