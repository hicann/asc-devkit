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
    "impl/tensor_api/arch/cube/l1_to_fb/copy.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file copy.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_L1_TO_FB_COPY_H
#define IMPL_TENSOR_API_ARCH_CUBE_L1_TO_FB_COPY_H

#include "impl/tensor_api/arch/cube/l1_to_fb/routing.h"

namespace asc {
namespace te {

constexpr copy_l1_to_fixbuf_trait DEFAULT_COPY_L1_FIXBUF_TRAIT;

struct copy_l1_to_fixbuf_trait_default {
    using trait_type = copy_l1_to_fixbuf_trait;
    static constexpr const trait_type value = DEFAULT_COPY_L1_FIXBUF_TRAIT;
};

struct copy_l1_to_fixbuf {
public:
    template <typename Tp, const Tp& traits, typename... Args>
    __aicore__ inline static void copy(const Args&... args)
    {
        if ASCEND_IS_AIC {
            data_copy_impl<traits, Args...>(args...);
        }
    }

private:
    template <const copy_l1_to_fixbuf_trait& trait = DEFAULT_COPY_L1_FIXBUF_TRAIT, typename T, typename U>
    __aicore__ inline static void data_copy_impl(const T& dst, const U& src)
    {
        using dst_pos = get_mem_location<T>;
        using src_pos = get_mem_location<U>;
        static_assert(Std::is_same_v<dst_pos, location::fixbuf>, "copy_l1_to_fixbuf requires destination on FIXBUF");
        static_assert(Std::is_same_v<src_pos, location::l1>, "copy_l1_to_fixbuf requires source on L1");
        using dst_layout = typename T::layout_type;
        using src_layout = typename U::layout_type;
        using dst_layout_ptn = get_layout_pattern<dst_layout>;
        using src_layout_ptn = get_layout_pattern<src_layout>;
        TENSOR_API_DEBUG_CHECK(debug_check_layout, dst.layout(), "dst", "copy_l1_to_fixbuf");
        TENSOR_API_DEBUG_CHECK(debug_check_layout, src.layout(), "src", "copy_l1_to_fixbuf");
        TENSOR_API_DEBUG_CHECK(debug_check_copy_size, src, dst, "copy_l1_to_fixbuf");
        using copy_l1_to_fb_impl = typename copy_l1_to_fixbuf_routing<CURRENT_ARCH_VERSION, dst_layout_ptn, src_layout_ptn>::type;
        copy_l1_to_fb_impl::template run<trait, T, U>(dst, src);
    }

    template <const copy_l1_to_fixbuf_trait& trait = DEFAULT_COPY_L1_FIXBUF_TRAIT, typename T, typename U,
        typename DstCoord, typename SrcCoord, typename ShapeType>
    __aicore__ inline static void data_copy_impl(
        const T& dst, const U& src, const DstCoord& coord_dst, const SrcCoord& coord_src, const ShapeType& copy_shape)
    {
        using dst_pos = get_mem_location<T>;
        using src_pos = get_mem_location<U>;
        static_assert(Std::is_same_v<dst_pos, location::fixbuf>, "copy_l1_to_fixbuf requires destination on FIXBUF");
        static_assert(Std::is_same_v<src_pos, location::l1>, "copy_l1_to_fixbuf requires source on L1");
        using dst_layout_ptn = get_layout_pattern<typename T::layout_type>;
        using src_layout_ptn = get_layout_pattern<typename U::layout_type>;
        using impl = typename copy_l1_to_fixbuf_routing<CURRENT_ARCH_VERSION, dst_layout_ptn, src_layout_ptn>::type;
        auto resolved_coord_dst = resolve_copy_coord(dst.layout(), copy_shape, coord_dst);
        auto resolved_coord_src = resolve_copy_coord(src.layout(), copy_shape, coord_src);
        impl::template run<trait, T, U>(dst, src, resolved_coord_dst, resolved_coord_src, copy_shape);
    }
};

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ARCH_CUBE_L1_TO_FB_COPY_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
