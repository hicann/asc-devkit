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
    "impl/tensor_api/arch/cube/l0c_to_ub/copy_impl/data_copy.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file data_copy.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_L0C_TO_UB_COPY_IMPL_DATA_COPY_H
#define IMPL_TENSOR_API_ARCH_CUBE_L0C_TO_UB_COPY_IMPL_DATA_COPY_H

#include "impl/tensor_api/utils/utils_impl.h"
#include "impl/tensor_api/arch/cube/l1_to_fb/copy.h"
#include "impl/tensor_api/arch/cube/utils/l0c2out_utils.h"
#include "impl/tensor_api/arch/cube/l0c_to_ub/copy_impl/instruction.h"

namespace asc {
namespace te {

struct copy_l0c_to_ub_trait {
    __aicore__ constexpr copy_l0c_to_ub_trait() {}

    __aicore__ constexpr copy_l0c_to_ub_trait(asc::te::round_mode round_mode, bool enable_relu,
                                              bool enable_channel_split, dual_dst_mode dual_dst_ctl) :
        round_mode(round_mode), enable_relu(enable_relu), enable_channel_split(enable_channel_split),
        dual_dst_ctl(dual_dst_ctl)
    {}

    asc::te::round_mode round_mode = asc::te::round_mode::default_round;
    bool enable_relu = false;
    bool enable_channel_split = false;
    dual_dst_mode dual_dst_ctl = dual_dst_mode::disable;
};

class data_copy_l0c_to_ub_no_vector_quant {
public:
    template <const copy_l0c_to_ub_trait& trait, QuantMode_t quant_pre, typename T, typename U>
    __aicore__ inline static void data_copy_impl(const T& dst, const U& src, const fixpipe_params& params)
    {
        if constexpr (is_l0c_out_src_batch_layout_v<U> && !is_l0c_out_batch_nz2nz_v<T, U>) {
            emit_data_copy<trait, quant_pre>(dst, src, get<1>(dst.layout()), get<1>(src.layout()), params);
        } else {
            emit_data_copy<trait, quant_pre>(dst, src, dst.layout(), src.layout(), params);
        }
    }

private:
    template <const copy_l0c_to_ub_trait& trait, QuantMode_t quant_pre, typename T, typename U, typename DstLayout,
              typename SrcLayout>
    __aicore__ inline static void emit_data_copy(const T& dst, const U& src, const DstLayout& dst_layout,
                                                 const SrcLayout& src_layout, const fixpipe_params& params)
    {
        constexpr bool is_nd_format = is_l0c_out_nd_format_v<T>;
        constexpr bool is_dn_format = is_l0c_out_dn_format_v<T>;
        auto copy_params = make_l0c_out_copy_params<T, U>(dst_layout, src_layout);

        bool relu_en = trait.enable_relu;
        uint8_t unit_flag = params.unit_flag;
        bool nz2nd_en = is_nd_format;
        bool nz2dn_en = is_dn_format;

        bool is_channel_split = trait.enable_channel_split;

        uint8_t dual_dst_ctl = trait.dual_dst_ctl;
        bool sub_block_id = params.sub_block_id;
        copy_l0c_to_ub_instr::data_copy<quant_pre>(
            dst.data().get(), src.data().get(), copy_params.n_size, copy_params.m_size, copy_params.src_stride,
            copy_params.dst_stride, dual_dst_ctl, relu_en, unit_flag, sub_block_id, is_channel_split, nz2nd_en,
            nz2dn_en);
    }
};

class data_copy_l0c_to_ub_vector_quant {
public:
    template <const copy_l0c_to_ub_trait& trait, QuantMode_t quant_pre, typename T, typename U, typename V>
    __aicore__ inline static void data_copy_impl(const T& dst, const U& src, const V& quant,
                                                 const fixpipe_params& params)
    {
        if constexpr (is_l0c_out_batch_nz2nz_v<T, U>) {
            emit_batch_nz2_nz_data_copy<trait, quant_pre>(dst, src, quant, params);
        } else if constexpr (is_l0c_out_src_batch_layout_v<U>) {
            emit_data_copy<trait, quant_pre>(dst, src, quant, get<1>(dst.layout()), get<1>(src.layout()), params);
        } else {
            emit_data_copy<trait, quant_pre>(dst, src, quant, dst.layout(), src.layout(), params);
        }
    }

private:
    template <const copy_l0c_to_ub_trait& trait, QuantMode_t quant_pre, typename T, typename U, typename V>
    __aicore__ inline static void emit_batch_nz2_nz_data_copy(const T& dst, const U& src, const V& quant,
                                                              const fixpipe_params& params)
    {
        auto dst_layout = dst.layout();
        auto src_layout = src.layout();
        for (uint32_t batch_index = 0; batch_index < get<0>(src_layout.shape()); ++batch_index) {
            emit_data_copy<trait, quant_pre>(
                dst(make_coord(batch_index, make_coord(make_coord(0, 0), make_coord(0, 0)))),
                src(make_coord(batch_index, make_coord(make_coord(0, 0), make_coord(0, 0)))), quant, get<1>(dst_layout),
                get<1>(src_layout), params);
        }
    }

    template <const copy_l0c_to_ub_trait& trait, QuantMode_t quant_pre, typename T, typename U, typename V,
              typename DstLayout, typename SrcLayout>
    __aicore__ inline static void emit_data_copy(const T& dst, const U& src, const V& quant,
                                                 const DstLayout& dst_layout, const SrcLayout& src_layout,
                                                 const fixpipe_params& params)
    {
        uint32_t n_size = Std::min(get_total_column_shape(src_layout), get_total_column_shape(dst_layout));
        uint16_t n_iter_num = 1;
        uint32_t cal_n_size = n_size;
        uint32_t tail_n_size = 0;
        if (cal_n_size > MAIN_LOOP_N_SIZE) {
            n_iter_num = n_size / MAIN_LOOP_N_SIZE;
            tail_n_size = n_size % MAIN_LOOP_N_SIZE;
            cal_n_size = MAIN_LOOP_N_SIZE;
        }
        execute_data_copy<trait, quant_pre>(dst, src, quant, n_iter_num, cal_n_size, tail_n_size, dst_layout,
                                            src_layout, params);
    }

    template <const copy_l0c_to_ub_trait& trait, typename T, bool is_tail, typename DstLayout, typename SrcLayout>
    __aicore__ inline static auto generate_params(const DstLayout& dst_layout, const SrcLayout& src_layout,
                                                  const fixpipe_params& params)
    {
        constexpr bool is_nd_format = is_l0c_out_nd_format_v<T>;
        constexpr bool is_dn_format = is_l0c_out_dn_format_v<T>;

        uint32_t n_size = Std::min(get_total_column_shape(src_layout), get_total_column_shape(dst_layout));
        uint32_t m_size = Std::min(get_total_row_shape(src_layout), get_total_row_shape(dst_layout));
        if constexpr (is_tail) {
            n_size %= MAIN_LOOP_N_SIZE;
        } else {
            if (n_size > MAIN_LOOP_N_SIZE) {
                n_size = MAIN_LOOP_N_SIZE;
            }
        }

        const uint32_t src_stride = get_element<attr_info::stride, attr_info::column, 1>(src_layout) / FRACTAL_FIXED;
        uint32_t dst_stride = 0;
        if constexpr (is_nd_format) {
            dst_stride = get_l0c_out_nd_stride<T>(dst_layout);
        } else if constexpr (is_dn_format) {
            dst_stride = get_l0c_out_dn_stride<T>(dst_layout);
        } else {
            dst_stride = get_element<attr_info::stride, attr_info::column, 1>(dst_layout);
        }

        const bool relu_en = trait.enable_relu;
        const uint8_t unit_flag = params.unit_flag;

        constexpr bool nz2nd_en = is_nd_format;
        constexpr bool nz2dn_en = is_dn_format;

        const bool channel_split = trait.enable_channel_split;
        bool sub_block_id = params.sub_block_id;
        uint8_t dual_dst_ctl = trait.dual_dst_ctl;

        return Std::make_tuple(n_size, m_size, src_stride, dst_stride, dual_dst_ctl, relu_en, unit_flag, sub_block_id,
                               channel_split, nz2nd_en, nz2dn_en);
    }

    template <typename T>
    __aicore__ inline static void copy_l1_to_fixbuf(const T& src, uint16_t cal_n_size, uint16_t n_iter_index)
    {
        auto dst_addr = reinterpret_cast<__fbuf__ uint64_t*>(alloc_fixbuf_temp_buf(cal_n_size));
        auto dst = make_tensor(make_mem_ptr<location::fixbuf>(dst_addr), src.layout());
        auto coord = make_coord(_0{}, n_iter_index * MAIN_LOOP_N_SIZE);
        auto shape = make_shape(_1{}, cal_n_size);
        auto tile_src = src.slice(coord, shape);
        copy_l1_to_fixbuf_nd::run<DEFAULT_COPY_L1_FIXBUF_TRAIT>(dst, tile_src);
        set_qua(dst_addr);
    }

    template <QuantMode_t quant_pre, typename T, typename U, typename ParamTuple, size_t... Is>
    __aicore__ inline static void data_copy_wrapper(const T& dst, const U& src, const ParamTuple& param_tuple,
                                                    Std::index_sequence<Is...>)
    {
        copy_l0c_to_ub_instr::data_copy<quant_pre>(dst.data().get(), src.data().get(), Std::get<Is>(param_tuple)...);
    }

    template <typename T>
    __aicore__ inline static constexpr auto make_dst_coord(uint32_t n_offset)
    {
        using layout_type = typename T::layout_type;
        if constexpr (layout_type::depth == FIVE_DIM_DATA) {
            return make_coord(_0{}, make_coord(make_coord(0, 0), make_coord(0, n_offset)));
        } else if constexpr (layout_type::depth == FOUR_DIM_DATA) {
            return make_coord(make_coord(0, 0), make_coord(0, n_offset));
        } else if constexpr (layout_type::depth == THREE_DIM_DATA) {
            return make_coord(_0{}, make_coord(0, n_offset));
        } else {
            static_assert(layout_type::depth == TWO_DIM_DATA, "Only support two-dim or four-dim dst tensor.");
            return make_coord(0, n_offset);
        }
    }

    template <typename U>
    __aicore__ inline static constexpr auto make_src_coord(uint32_t n_offset)
    {
        using layout_type = typename U::layout_type;
        if constexpr (layout_type::depth == FIVE_DIM_DATA) {
            return make_coord(_0{}, make_coord(make_coord(0, 0), make_coord(0, n_offset)));
        } else {
            static_assert(layout_type::depth == FOUR_DIM_DATA, "Only support four-dim or five-dim src tensor.");
            return make_coord(make_coord(0, 0), make_coord(0, n_offset));
        }
    }

    template <const copy_l0c_to_ub_trait& trait, QuantMode_t quant_pre, typename T, typename U, typename V,
              typename DstLayout, typename SrcLayout>
    __aicore__ inline static void execute_data_copy(const T& dst, const U& src, const V& quant, uint16_t n_iter_num,
                                                    uint32_t cal_n_size, uint32_t tail_n_size,
                                                    const DstLayout& dst_layout, const SrcLayout& src_layout,
                                                    const fixpipe_params& params)
    {
        const auto main_loop_param = generate_params<trait, T, false>(dst_layout, src_layout, params);

        for (uint16_t i = 0; i < n_iter_num; ++i) {
            copy_l1_to_fixbuf(quant, cal_n_size, i);
            insert_sync();

            const auto src_coord = make_src_coord<U>(i * CBURST_NUM);
            const auto dst_coord = make_dst_coord<T>(i * MAIN_LOOP_N_SIZE);

            data_copy_wrapper<quant_pre>(dst(dst_coord), src(src_coord), main_loop_param,
                                         Std::make_index_sequence<Std::tuple_size_v<decltype(main_loop_param)>>{});
        }

        if (tail_n_size) {
            const auto tail_param = generate_params<trait, T, true>(dst_layout, src_layout, params);

            copy_l1_to_fixbuf(quant, tail_n_size, n_iter_num);
            insert_sync();

            const auto src_coord = make_src_coord<U>(n_iter_num * CBURST_NUM);
            const auto dst_coord = make_dst_coord<T>(n_iter_num * MAIN_LOOP_N_SIZE);

            data_copy_wrapper<quant_pre>(dst(dst_coord), src(src_coord), tail_param,
                                         Std::make_index_sequence<Std::tuple_size_v<decltype(tail_param)>>{});
        }
    }
};

class data_copy_l0c_to_ub {
public:
    template <const copy_l0c_to_ub_trait& trait, typename T, typename U>
    __aicore__ inline static void run(const T& dst, const U& src, const fixpipe_params& params)
    {
        constexpr QuantMode_t quant_pre = get_quant_mode<trait.round_mode, T, U>();
        check_data_type::check_l0c_to_ub_data_type<quant_pre, T, U>();
        set_register_impl<T, U>(dst, src);
        data_copy_l0c_to_ub_no_vector_quant::data_copy_impl<trait, quant_pre, T, U>(dst, src, params);
    }

    template <const copy_l0c_to_ub_trait& trait, typename T, typename U>
    __aicore__ inline static void run(const T& dst, const U& src, uint64_t quant, const fixpipe_params& params)
    {
        constexpr QuantMode_t quant_pre = get_quant_mode<trait.round_mode, T, U, uint64_t>();
        set_register_impl<T, U>(dst, src, quant);
        data_copy_l0c_to_ub_no_vector_quant::data_copy_impl<trait, quant_pre, T, U>(dst, src, params);
    }

    template <const copy_l0c_to_ub_trait& trait, typename T, typename U, typename V>
    __aicore__ inline static void run(const T& dst, const U& src, const V& quant, const fixpipe_params& params)
    {
        constexpr QuantMode_t quant_pre = get_quant_mode<trait.round_mode, T, U, V>();
        constexpr bool quant_batched = check_vector_quant_batch_consistency<T, U, V>();

        if constexpr (quant_batched) {
            // Fixpipe hardware reads a single quant slice from FB per instruction and reuses it
            // across the nd_num batch repetition; the FB quant address does not auto-step per
            // batch. To honor per-batch quant we must split on the host: each batch issues its
            // own fixpipe instruction with its own L1->FB quant copy.
            uint32_t batch_size = get<0>(src.layout().shape());
            for (uint32_t b = 0; b < batch_size; ++b) {
                auto sub_dst = make_single_batch_sub_tensor(dst, b);
                auto sub_src = make_single_batch_sub_tensor(src, b);
                auto sub_quant = make_single_batch_sub_tensor(quant, b);
                set_register_impl<decltype(sub_dst), decltype(sub_src)>(sub_dst, sub_src);
                data_copy_l0c_to_ub_vector_quant::data_copy_impl<trait, quant_pre, decltype(sub_dst), decltype(sub_src),
                                                                 decltype(sub_quant)>(sub_dst, sub_src, sub_quant,
                                                                                      params);
            }
        } else {
            set_register_impl<T, U>(dst, src);
            data_copy_l0c_to_ub_vector_quant::data_copy_impl<trait, quant_pre, T, U, V>(dst, src, quant, params);
        }
    }
};

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ARCH_CUBE_L0C_TO_UB_COPY_IMPL_DATA_COPY_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
