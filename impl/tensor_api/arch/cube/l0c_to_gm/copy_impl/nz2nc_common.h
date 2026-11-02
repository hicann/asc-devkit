/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef IMPL_TENSOR_API_ARCH_CUBE_L0C_TO_GM_COPY_IMPL_NZ2NC_COMMON_H
#define IMPL_TENSOR_API_ARCH_CUBE_L0C_TO_GM_COPY_IMPL_NZ2NC_COMMON_H

#include "impl/tensor_api/arch/cube/l0c_to_gm/copy_impl/instruction.h"

namespace asc {
namespace te {

template <bool is3d>
class data_copy_l0c_to_gm_nz2nc_base {
public:
    template <const l0c_to_gm_trait& trait, typename DstTensor, typename SrcTensor>
    __aicore__ inline static void run(const DstTensor& dst, const SrcTensor& src, const l0c_to_gm_params& params)
    {
        constexpr QuantMode_t quant_pre = get_quant_mode<trait.round_mode, DstTensor, SrcTensor>();
        check_data_type::check_l0c_to_gm_data_type<quant_pre, DstTensor, SrcTensor>();

        auto dst_layout = dst.layout();
        auto src_layout = src.layout();

        set_register_instr::set_register(1u, 0u, 0u, get_element<attr_info::stride, attr_info::column, 0>(src_layout));

        uint32_t n_size = get<1>(dst_layout.shape());
        uint32_t m_size = get<2>(dst_layout.shape()) * get<3>(dst_layout.shape());
        if constexpr (is3d) {
            m_size *= get<4>(dst_layout.shape());
        }
        uint32_t src_stride = get_element<attr_info::stride, attr_info::column, 1>(src_layout) / fractal_fixed;
        uint32_t dst_stride = get<1>(dst_layout.stride());

        auto cache_mode = static_cast<asc_store_l2_cache_mode>(dst.engine().get_cache_mode());
        bool relu_en = trait.enable_relu;
        auto unit_flag = static_cast<asc_unit_flag_mode>(params.unit_flag);
        bool is_channel_split = trait.enable_channel_split;

        copy_l0c_to_gm_instr::data_copy<quant_pre>(
            dst.data().get(), src.data().get(), n_size, m_size, src_stride, dst_stride, cache_mode, relu_en, unit_flag,
            is_channel_split, false, true);
    }

    template <
        const l0c_to_gm_trait& trait, typename DstTensor, typename SrcTensor, typename DstCoord, typename SrcCoord,
        typename CopyShape>
    __aicore__ inline static void run(
        const DstTensor& dst, const SrcTensor& src, const DstCoord& dst_coord, const SrcCoord& src_coord,
        const CopyShape& copy_shape, const l0c_to_gm_params& params)
    {
        constexpr QuantMode_t quant_pre = get_quant_mode<trait.round_mode, DstTensor, SrcTensor>();
        check_data_type::check_l0c_to_gm_data_type<quant_pre, DstTensor, SrcTensor>();
        auto src_shape = make_slice_shape(src_coord, src.layout(), copy_shape);
        auto dst_offset = dst.layout()(dst_coord);
        auto src_offset = src.layout()(src_coord);
        auto src_layout = src.layout();
        set_register_instr::set_register(1u, 0u, 0u, get_element<attr_info::stride, attr_info::column, 0>(src_layout));
        uint32_t n_size = get_shape_columns(src_shape);
        uint32_t m_size = get_shape_rows(src_shape);
        uint32_t src_stride = get_element<attr_info::stride, attr_info::column, 1>(src_layout) / fractal_fixed;
        uint32_t dst_stride = get<1>(dst.layout().stride());
        copy_l0c_to_gm_instr::data_copy_with_offset<quant_pre>(
            dst, src, dst_offset, src_offset, n_size, m_size, src_stride, dst_stride,
            static_cast<asc_store_l2_cache_mode>(dst.engine().get_cache_mode()), trait.enable_relu,
            static_cast<asc_unit_flag_mode>(params.unit_flag), trait.enable_channel_split, false, true);
    }
};

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ARCH_CUBE_L0C_TO_GM_COPY_IMPL_NZ2NC_COMMON_H
