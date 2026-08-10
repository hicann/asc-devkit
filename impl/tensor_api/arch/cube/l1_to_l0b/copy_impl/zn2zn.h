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
    "impl/tensor_api/arch/cube/l1_to_l0b/copy_impl/zn2zn.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file zn2zn.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0B_COPY_IMPL_ZN2ZN_H
#define IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0B_COPY_IMPL_ZN2ZN_H

#include "impl/tensor_api/arch/cube/l1_to_l0b/copy_impl/instruction.h"

namespace asc {
namespace te {
class load_data_l1_to_l0b_zn2zn {
public:
    template <const copy_l1_to_l0b_trait& trait, typename T, typename U>
    __aicore__ inline static void run(const T& dst, const U& src)
    {
        check_template<trait, T, U>();
        if constexpr (T::layout_type::depth == FIVE_DIM_DATA) {
            batch_load_data_impl<trait, T, U>(dst, src);
        } else if constexpr (T::layout_type::depth == FOUR_DIM_DATA) {
            load_data_impl<trait, T, U>(dst, src);
        } else {
            static_assert(T::layout_type::depth == FOUR_DIM_DATA || T::layout_type::depth == FIVE_DIM_DATA,
                          "load_data_l1_to_l0b_zn2zn only supports the plain fractal layout "
                          "((col0,col1),(row0,row1)) or the batch layout (B,((col0,col1),(row0,row1))).");
        }
    }

private:
    template <const copy_l1_to_l0b_trait& trait, typename T, typename U>
    __aicore__ inline static constexpr void check_template()
    {
        check_layout_pattern<T, U>();
        check_data_type::check_l1_to_l0b_data_type<T, U>();
    }

    template <const copy_l1_to_l0b_trait& trait, typename T, typename U>
    __aicore__ inline static void load_data_impl(const T& dst, const U& src)
    {
        using dst_type = typename T::element_type;
        auto dst_layout = dst.layout();
        auto src_layout = src.layout();
        uint16_t m_start_position = 0;
        uint16_t k_start_position = 0;
        auto m_step = get_element<attr_info::shape, attr_info::column, 1>(dst_layout);
        auto k_step = get_element<attr_info::shape, attr_info::row, 1>(dst_layout);
        // Zn -> Zn
        constexpr uint32_t STRIDE_UNIT = C0_ELEMENT<dst_type> * FRACTAL_FIXED;
        auto src_stride = get_element<attr_info::stride, attr_info::row, 1>(src_layout) / STRIDE_UNIT;
        auto dst_stride = get_element<attr_info::stride, attr_info::row, 1>(dst_layout) / STRIDE_UNIT;
        load_l1_to_l0b_instr::load_data<false>(dst.data().get(), src.data().get(), m_start_position,
                                                 k_start_position, m_step, k_step, src_stride, dst_stride);
    }

    template <const copy_l1_to_l0b_trait& trait, typename T, typename U>
    __aicore__ inline static void batch_load_data_impl(const T& dst, const U& src)
    {
        using dst_type = typename T::element_type;
        auto dst_layout = dst.layout();
        auto src_layout = src.layout();
        auto dst_no_batch_layout = remove_batch_dim(dst_layout);
        auto src_no_batch_layout = remove_batch_dim(src_layout);
        uint16_t m_start_position = 0;
        uint16_t k_start_position = 0;
        auto m_step = get_element<attr_info::shape, attr_info::column, 1>(dst_no_batch_layout);
        auto k_step = get<0>(dst_layout.shape()) * get_element<attr_info::shape, attr_info::row, 1>(dst_no_batch_layout);
        // Zn -> Zn
        constexpr uint32_t STRIDE_UNIT = C0_ELEMENT<dst_type> * FRACTAL_FIXED;
        auto src_stride = get_element<attr_info::stride, attr_info::row, 1>(src_no_batch_layout) / STRIDE_UNIT;
        auto dst_stride = get_element<attr_info::stride, attr_info::row, 1>(dst_no_batch_layout) / STRIDE_UNIT;
        load_l1_to_l0b_instr::load_data<false>(dst.data().get(), src.data().get(), m_start_position,
                                                 k_start_position, m_step, k_step, src_stride, dst_stride);
    }
};
} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0B_COPY_IMPL_ZN2ZN_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
