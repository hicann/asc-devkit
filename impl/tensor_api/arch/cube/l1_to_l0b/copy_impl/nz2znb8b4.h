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
    "impl/tensor_api/arch/cube/l1_to_l0b/copy_impl/nz2znb8b4.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file nz2znb8b4.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0B_COPY_IMPL_NZ2ZNB8B4_H
#define IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0B_COPY_IMPL_NZ2ZNB8B4_H

#include "impl/tensor_api/arch/cube/l1_to_l0b/copy_impl/instruction.h"

namespace asc {
namespace te {
class load_data_l1_to_l0b_nz2zn_b8b4 {
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
                          "load_data_l1_to_l0b_nz2zn_b8b4 only supports the plain fractal layout "
                          "((col0,col1),(row0,row1)) or the batch layout (B,((col0,col1),(row0,row1))).");
        }
    }

private:
    static constexpr uint8_t M_STEP_MIN_VAL_B4 = 4;
    static constexpr uint8_t M_STEP_MIN_VAL_B8 = 2;

    template <const copy_l1_to_l0b_trait& trait, typename T, typename U>
    __aicore__ inline static constexpr void check_template()
    {
        check_layout_pattern<T, U>();
        check_data_type::check_l1_to_l0b_data_type<T, U>();
    }

    template <uint8_t m_step_min_val, typename T, typename U>
    __aicore__ inline static void load_data_impl_split(const T& dst, const U& src, uint16_t m_start_position,
                                                       uint16_t k_start_position, uint8_t m_step, uint8_t k_step,
                                                       int16_t src_stride, uint16_t dst_stride)
    {
        uint16_t n_loop = m_step / m_step_min_val;
        m_step = m_step_min_val;
        for (uint16_t idx = 0; idx < n_loop; ++idx) {
            auto slice_dst = dst(make_coord(make_coord(0, idx), make_coord(0, 0)));
            load_l1_to_l0b_instr::load_data<true>(slice_dst.data().get(), src.data().get(), m_start_position,
                                                    k_start_position, m_step, k_step, src_stride, dst_stride);
            m_start_position += m_step_min_val;
        }
    }

    template <typename DstT, typename SrcT, typename DstLayoutT, typename SrcLayoutT>
    __aicore__ inline static void load_data_fractal(const DstT& dst, const SrcT& src, const DstLayoutT& dst_layout,
                                                    const SrcLayoutT& src_layout)
    {
        using dst_type = typename DstT::element_type;
        uint16_t m_start_position = 0;
        uint16_t k_start_position = 0;
        auto n1 = get_element<attr_info::shape, attr_info::column, 1>(src_layout)
                      * get_element<attr_info::shape, attr_info::column, 0>(src_layout)
                  - get_element<attr_info::shape, attr_info::column, 1>(dst_layout)
                        * get_element<attr_info::shape, attr_info::column, 0>(dst_layout);
        auto m_step = get_element<attr_info::shape, attr_info::row, 1>(dst_layout)
                      * get_element<attr_info::shape, attr_info::row, 0>(dst_layout) / FRACTAL_FIXED;
        auto k_step = get_element<attr_info::shape, attr_info::column, 1>(src_layout);
        // Nz -> Zn
        constexpr uint32_t STRIDE_UNIT = C0_ELEMENT<dst_type> * FRACTAL_FIXED;
        auto src_stride = get_element<attr_info::stride, attr_info::column, 1>(src_layout) / STRIDE_UNIT;
        auto dst_stride = get_element<attr_info::stride, attr_info::row, 1>(dst_layout) / STRIDE_UNIT;
        if (n1 < FRACTAL_FIXED) {
            load_l1_to_l0b_instr::load_data<true>(dst.data().get(), src.data().get(), m_start_position,
                                                    k_start_position, m_step, k_step, src_stride, dst_stride);
        } else if constexpr (is_b4_type<dst_type>) {
            load_data_impl_split<M_STEP_MIN_VAL_B4>(dst, src, m_start_position, k_start_position, m_step, k_step,
                                                    src_stride, dst_stride);
        } else {
            load_data_impl_split<M_STEP_MIN_VAL_B8>(dst, src, m_start_position, k_start_position, m_step, k_step,
                                                    src_stride, dst_stride);
        }
    }

    template <const copy_l1_to_l0b_trait& trait, typename T, typename U>
    __aicore__ inline static void load_data_impl(const T& dst, const U& src)
    {
        load_data_fractal(dst, src, dst.layout(), src.layout());
    }

    template <const copy_l1_to_l0b_trait& trait, typename T, typename U>
    __aicore__ inline static void batch_load_data_impl(const T& dst, const U& src)
    {
        auto dst_layout = dst.layout();
        auto src_layout = src.layout();
        auto dst_no_batch_layout = remove_batch_dim(dst_layout);
        auto src_no_batch_layout = remove_batch_dim(src_layout);
        auto batch_num = get<0>(dst_layout.shape());
        for (uint32_t i = 0; i < batch_num; ++i) {
            load_data_fractal(make_single_batch_sub_tensor(dst, i), make_single_batch_sub_tensor(src, i),
                              dst_no_batch_layout, src_no_batch_layout);
        }
    }
};
} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0B_COPY_IMPL_NZ2ZNB8B4_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
