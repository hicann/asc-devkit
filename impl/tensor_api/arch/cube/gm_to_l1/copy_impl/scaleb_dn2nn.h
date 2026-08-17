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
    "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/scaleb_dn2nn.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file scaleb_dn2nn.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_SCALEB_DN2NN_H
#define IMPL_TENSOR_API_ARCH_CUBE_GM_TO_L1_COPY_IMPL_SCALEB_DN2NN_H

#include "impl/tensor_api/arch/cube/gm_to_l1/copy_impl/copy_common.h"

namespace asc {
namespace te {

class copy_gm_to_l1_scaleb_dn2nn {
public:
    template <const copy_gm_to_l1_trait& trait, typename T, typename U>
    __aicore__ inline static void run(const T& dst, const U& src)
    {
        if constexpr (U::layout_type::depth == THREE_DIM_DATA || U::layout_type::depth == FIVE_DIM_DATA) {
            TENSOR_API_DEBUG_CHECK(debug_check_batch_count, get<0>(src.layout().shape()),
                                   "copy_gm_to_l1 scaleb_dn2nn path");
        }
        run_gm_to_l1_batched<trait, copy_gm_to_l1_scaleb_dn2nn, T, U>(dst, src);
    }

    template <const copy_gm_to_l1_trait& trait, typename T, typename U, typename DstCoord, typename SrcCoord, typename ShapeType>
    __aicore__ inline static void run(
        const T& dst, const U& src, const DstCoord& coord_dst, const SrcCoord& coord_src, const ShapeType& copy_shape)
    {
        check_template<trait, T, U>();
        using type = typename U::element_type;
        auto src_shape = make_slice_shape(coord_src, src.layout(), copy_shape);
        auto dst_offset = dst.layout()(coord_dst);
        auto src_offset = src.layout()(coord_src);
        uint16_t matrix_num = get_shape_batch_size(src_shape);
        uint16_t n_value = get_shape_rows(src_shape) >> 1;
        uint32_t d_value = get_shape_columns(src_shape);
        uint32_t src_col_stride = get_matrix_element<attr_info::stride, attr_info::column, 1>(src.layout());
        uint16_t dst_stride = get_matrix_element<attr_info::stride, attr_info::column, 1>(dst.layout()) *
            sizeof(type) / C0_SIZE<>;
        uint64_t src_matrix_stride = 0;
        uint16_t dst_matrix_stride = 0;
        if constexpr (U::layout_type::depth == THREE_DIM_DATA) {
            src_matrix_stride = get<0>(src.layout().stride()) * sizeof(type);
            dst_matrix_stride = get<0>(dst.layout().stride()) * sizeof(type) / C0_SIZE<>;
        }
        copy_gm_to_l1_multi_dn2nz_instr::data_copy_with_offset(dst, src, dst_offset, src_offset, matrix_num, 1, dst_stride,
            dst_matrix_stride, src_col_stride * sizeof(type), src.engine().get_cache_mode(), n_value,
            d_value, src_matrix_stride, false);
    }

    template <const copy_gm_to_l1_trait& trait, typename T, typename U>
    __aicore__ inline static constexpr void check_template()
    {
        check_layout_pattern<U, T>();
        check_data_type::check_gm_to_l1_scale_data_type<T, U>();
    }

    template <typename T, typename U, typename SrcLayout, typename DstLayout>
    __aicore__ inline static void emit_copy(const T& dst, const U& src, const SrcLayout& src_layout,
        const DstLayout& dst_layout, uint16_t dn_num, uint64_t src_dn_matrix_stride, uint32_t dst_nz_matrix_stride)
    {
        using type = typename U::element_type;

        uint32_t src_row_shape;
        uint32_t src_col_shape;
        uint16_t src_b_col_stride;
        if constexpr (is_satisfied_ptn_format_v<U, dn_layout_ptn>) {
            src_row_shape = get_element<attr_info::shape, attr_info::row>(src_layout);
            src_col_shape = get_element<attr_info::shape, attr_info::column>(src_layout);
            src_b_col_stride = get_element<attr_info::stride, attr_info::column>(src_layout);
        } else {
            src_row_shape = get_element<attr_info::shape, attr_info::row, 1>(src_layout);
            src_col_shape = get_element<attr_info::shape, attr_info::column, 1>(src_layout);
            src_b_col_stride = get_element<attr_info::stride, attr_info::column, 1>(src_layout);
        }
        uint16_t dst_b_col_stride =
            get_element<attr_info::stride, attr_info::column, 1>(dst_layout);

        uint16_t n_value = src_row_shape >> 1; // use b16 for DN2NZ, so n_value = src_row_shape / 2
        uint16_t d_value = src_col_shape;
        uint16_t dst_nz_n_stride = 1;

        uint64_t loop1_src_stride = src_b_col_stride * sizeof(type);
        uint64_t loop4_src_stride = src_dn_matrix_stride * sizeof(type);

        uint16_t loop2_dst_stride = dst_nz_n_stride;                             // loop2_dst_stride = dst_nz_n_stride
        uint16_t loop3_dst_stride = dst_b_col_stride * sizeof(type) / C0_SIZE<>; // loop3_dst_stride = dst_nz_c0_Stride
        uint16_t loop4_dst_stride = dst_nz_matrix_stride * sizeof(type) / C0_SIZE<>;
        uint8_t cache_mode = src.engine().get_cache_mode();
        // fp8 scale use b16 for movement
        copy_gm_to_l1_multi_dn2nz_instr::data_copy(
            (__cbuf__ half*)(dst.data().get()), (__gm__ half*)(src.data().get()), dn_num, loop2_dst_stride, loop3_dst_stride,
            loop4_dst_stride, loop1_src_stride, cache_mode, n_value, d_value, loop4_src_stride, false);
    }
};

} // namespace te
} // namespace asc

#endif

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
