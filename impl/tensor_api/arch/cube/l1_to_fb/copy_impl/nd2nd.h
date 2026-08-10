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
    "impl/tensor_api/arch/cube/l1_to_fb/copy_impl/nd2nd.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file nd2nd.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_L1_TO_FB_COPY_IMPL_ND2ND_H
#define IMPL_TENSOR_API_ARCH_CUBE_L1_TO_FB_COPY_IMPL_ND2ND_H

#include "impl/tensor_api/arch/cube/l1_to_fb/copy_impl/instruction.h"

namespace asc {
namespace te {

class copy_l1_to_fixbuf_nd {
public:
    template <const copy_l1_to_fixbuf_trait& trait, typename T, typename U>
    __aicore__ inline static void run(const T& dst, const U& src)
    {
        data_copy_impl<trait, T, U>(dst, src);
    }

private:
    template <const copy_l1_to_fixbuf_trait& trait, typename T, typename U>
    __aicore__ inline static constexpr void check_template()
    {
        check_data_type::check_l1_to_fixbuf_data_type<T, U>();
        check_layout_pattern<U, T>();
    }

    template <const copy_l1_to_fixbuf_trait& trait, typename T, typename U>
    __aicore__ inline static void data_copy_impl(const T& dst, const U& src)
    {
        check_template<trait, T, U>();

        using src_type = typename U::element_type;
        using dst_type = typename T::element_type;

        constexpr uint32_t deq_tensor_addr_align_value = 128;
        constexpr uint16_t fbuf_burst_len_unit = 64;

        auto dst_layout = dst.layout();
        auto src_layout = src.layout();

        uint16_t src_col;
        uint16_t src_row;
        uint16_t dst_col;
        uint16_t dst_row;
        uint16_t block_count;
        if constexpr (is_satisfied_ptn_format_v<U, nd_layout_ptn>) {
            src_col = get_element<attr_info::shape, attr_info::column>(src_layout);
            src_row = get_element<attr_info::stride, attr_info::row>(src_layout);
            block_count = get_element<attr_info::shape, attr_info::row>(src_layout);
        } else {
            src_col = get_element<attr_info::shape, attr_info::column, 1>(src_layout);
            src_row = get_element<attr_info::stride, attr_info::row, 1>(src_layout);
            block_count = get_element<attr_info::shape, attr_info::row, 1>(src_layout);
        }
        if constexpr (is_satisfied_ptn_format_v<T, nd_layout_ptn>) {
            dst_col = get_element<attr_info::shape, attr_info::column>(dst_layout);
            dst_row = get_element<attr_info::stride, attr_info::row>(dst_layout);
        } else {
            dst_col = get_element<attr_info::shape, attr_info::column, 1>(dst_layout);
            dst_row = get_element<attr_info::stride, attr_info::row, 1>(dst_layout);
        }

        uint16_t block_len =
            Std::ceil_align(src_col * sizeof(src_type), deq_tensor_addr_align_value) / fbuf_burst_len_unit;
        uint16_t src_stride = Std::ceil_division(src_row * sizeof(src_type), C0_SIZE<>);
        uint16_t dst_stride =
            Std::ceil_align(dst_row * sizeof(dst_type), deq_tensor_addr_align_value) / fbuf_burst_len_unit;

        copy_l1_to_fixbuf_instr::data_copy(dst.data().get(), src.data().get(), block_count, block_len, src_stride,
                                              dst_stride);
    }
};

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ARCH_CUBE_L1_TO_FB_COPY_IMPL_ND2ND_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
