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
    "impl/tensor_api/utils/npu_debug_utils.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define TENSOR_API_DEBUG_UTILS_OWNS_INTERNAL_HEADER_ACCESS
#endif

/*!
 * \file npu_debug_utils.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_UTILS_NPU_DEBUG_UTILS_H
#define IMPL_TENSOR_API_UTILS_NPU_DEBUG_UTILS_H

#ifdef ASCENDC_DEBUG
#include "impl/tensor_api/tensor/layout_pattern.h"

namespace asc {
namespace te {

constexpr auto DEBUG_MMAD_DIM_MIN = 0;
constexpr auto DEBUG_MMAD_DIM_MAX = 4095;
constexpr auto DEBUG_GEMV_M_SIZE = 1;
constexpr auto DEBUG_UNIT_FLAG_DEFAULT = 0;
constexpr auto DEBUG_UNIT_FLAG_KEEP = 2;
constexpr auto DEBUG_UNIT_FLAG_RESET = 3;
constexpr auto DEBUG_BLOCK_COUNT_MIN = 1;
constexpr auto DEBUG_BLOCK_COUNT_MAX = 4095;
constexpr auto DEBUG_BLOCK_LEN_MIN = 1;
constexpr auto DEBUG_BLOCK_LEN_MAX = 65535;
constexpr auto DEBUG_GM_UB_BLOCK_LEN_MAX = 2097151;
constexpr auto DEBUG_GM2UB_PADDING_BYTES_MAX = 32;
constexpr auto DEBUG_COPY_NOP_VALUE = 0;
constexpr auto DEBUG_SINGLE_BLOCK_COUNT = 1;
constexpr auto DEBUG_DATA_BLOCK_BYTES = 32;
constexpr auto DEBUG_BATCH_COUNT_MAX = 4095;
constexpr auto DEBUG_FIXPIPE_SRC_STRIDE_MAX = 65535;
constexpr auto DEBUG_FIXPIPE_M_MIN = 1;
constexpr auto DEBUG_FIXPIPE_M_ND_MAX = 8192;
constexpr auto DEBUG_FIXPIPE_M_MAX = 65535;
constexpr auto DEBUG_FIXPIPE_SPLIT_N_MULTIPLE = 8;
constexpr auto DEBUG_FIXPIPE_N_MULTIPLE = 16;
constexpr auto DEBUG_L0_STEP_MAX = 255;
constexpr auto DEBUG_L0_B4_M_STEP_MULTIPLE = 4;
constexpr auto DEBUG_L0_B8_M_STEP_MULTIPLE = 2;
constexpr auto DEBUG_L0_TRANSPOSE_DATA_BYTES = 4;
constexpr auto DEBUG_L0_TRANSPOSE_K_STEP_MULTIPLE = 2;
constexpr auto DEBUG_IMG2COL_M_START_MAX = 32767;
constexpr auto DEBUG_IMG2COL_B32_K_START_MULTIPLE = 8;
constexpr auto DEBUG_IMG2COL_B16_K_START_MULTIPLE = 16;
constexpr auto DEBUG_IMG2COL_B8_K_START_MULTIPLE = 32;
constexpr auto DEBUG_IMG2COL_B32_DATA_BYTES = 4;
constexpr auto DEBUG_IMG2COL_B16_DATA_BYTES = 2;
constexpr auto DEBUG_IMG2COL_DATA_BITS_PER_BYTE = 8;
constexpr auto DEBUG_IMG2COL_ALIGNED_REMAINDER = 0;

template <typename ElementType>
constexpr auto DEBUG_IMG2COL_K_START_MULTIPLE =
    sizeof(ElementType) == DEBUG_IMG2COL_B32_DATA_BYTES ?
        DEBUG_IMG2COL_B32_K_START_MULTIPLE :
        (sizeof(ElementType) == DEBUG_IMG2COL_B16_DATA_BYTES ? DEBUG_IMG2COL_B16_K_START_MULTIPLE :
                                                               DEBUG_IMG2COL_B8_K_START_MULTIPLE);

template <typename ShapeType>
__aicore__ inline constexpr bool is_shape_valid(const ShapeType& shape);

template <typename ShapeType, size_t... indices>
__aicore__ inline constexpr bool is_shape_valid_impl(const ShapeType& shape, Std::index_sequence<indices...>)
{
    return (is_shape_valid(Std::get<indices>(shape)) && ...);
}

template <typename ShapeType>
__aicore__ inline constexpr bool is_shape_valid(const ShapeType& shape)
{
    if constexpr (Std::is_tuple_v<Std::remove_cvref_t<ShapeType>>) {
        return is_shape_valid_impl(shape,
                                   Std::make_index_sequence<Std::tuple_size_v<Std::remove_cvref_t<ShapeType>>>{});
    } else {
        return shape > 0;
    }
}

template <typename StrideType>
__aicore__ inline constexpr bool is_stride_valid(const StrideType& stride);

template <typename StrideType, size_t... indices>
__aicore__ inline constexpr bool is_stride_valid_impl(const StrideType& stride, Std::index_sequence<indices...>)
{
    return (is_stride_valid(Std::get<indices>(stride)) && ...);
}

template <typename StrideType>
__aicore__ inline constexpr bool is_stride_valid(const StrideType& stride)
{
    if constexpr (Std::is_tuple_v<Std::remove_cvref_t<StrideType>>) {
        return is_stride_valid_impl(stride,
                                    Std::make_index_sequence<Std::tuple_size_v<Std::remove_cvref_t<StrideType>>>{});
    } else {
        return stride >= 0;
    }
}

template <typename CoordType, typename ShapeType>
__aicore__ inline constexpr bool is_coord_in_shape(const CoordType& coord, const ShapeType& shape);

template <typename CoordType, typename ShapeType, size_t... indices>
__aicore__ inline constexpr bool is_coord_in_shape_impl(const CoordType& coord, const ShapeType& shape,
                                                        Std::index_sequence<indices...>)
{
    return (is_coord_in_shape(Std::get<indices>(coord), Std::get<indices>(shape)) && ...);
}

template <typename CoordType, typename ShapeType>
__aicore__ inline constexpr bool is_coord_in_shape(const CoordType& coord, const ShapeType& shape)
{
    if constexpr (Std::is_tuple_v<Std::remove_cvref_t<CoordType>>) {
        static_assert(Std::is_tuple_v<Std::remove_cvref_t<ShapeType>>,
                      "The coord and shape must describe compatible dimensions.");
        if constexpr (Std::is_tuple_v<Std::remove_cvref_t<ShapeType>>) {
            static_assert(Std::tuple_size_v<Std::remove_cvref_t<CoordType>>
                              == Std::tuple_size_v<Std::remove_cvref_t<ShapeType>>,
                          "The coord and shape must have the same rank.");
            if constexpr (Std::tuple_size_v<Std::remove_cvref_t<CoordType>>
                          == Std::tuple_size_v<Std::remove_cvref_t<ShapeType>>) {
                return is_coord_in_shape_impl(
                    coord, shape, Std::make_index_sequence<Std::tuple_size_v<Std::remove_cvref_t<CoordType>>>{});
            }
        }
        return false;
    } else if constexpr (Std::is_tuple_v<Std::remove_cvref_t<ShapeType>>) {
        return coord >= 0 && coord < product{}(shape);
    } else {
        return coord >= 0 && coord < shape;
    }
}

template <typename InfoType>
__aicore__ inline constexpr bool is_slice_shape_valid(const InfoType& info)
{
    if constexpr (is_layout_v<InfoType>) {
        return is_shape_valid(info.shape());
    } else {
        return is_shape_valid(info);
    }
}

struct debug_tuple_unsupported {};
struct debug_tuple_flat_1 {};
struct debug_tuple_flat_2 {};
struct debug_tuple_flat_3 {};
struct debug_tuple_flat_4 {};
struct debug_tuple_flat_5 {};
struct debug_tuple_flat_6 {};
struct debug_tuple_nested_2x2 {};
struct debug_tuple_leading_scalar_flat_2 {};
struct debug_tuple_leading_scalar_nested_2x2 {};

template <size_t size>
struct debug_flat_tuple_format {
    using type = debug_tuple_unsupported;
};

#define TENSOR_API_DETAIL_DEFINE_FLAT_TUPLE_FORMAT(size)                                                               \
    template <>                                                                                                        \
    struct debug_flat_tuple_format<size> {                                                                             \
        using type = debug_tuple_flat_##size;                                                                          \
    }

TENSOR_API_DETAIL_DEFINE_FLAT_TUPLE_FORMAT(1);
TENSOR_API_DETAIL_DEFINE_FLAT_TUPLE_FORMAT(2);
TENSOR_API_DETAIL_DEFINE_FLAT_TUPLE_FORMAT(3);
TENSOR_API_DETAIL_DEFINE_FLAT_TUPLE_FORMAT(4);
TENSOR_API_DETAIL_DEFINE_FLAT_TUPLE_FORMAT(5);
TENSOR_API_DETAIL_DEFINE_FLAT_TUPLE_FORMAT(6);

#undef TENSOR_API_DETAIL_DEFINE_FLAT_TUPLE_FORMAT

template <typename... ValueTypes>
constexpr bool are_debug_scalar_values_v = (... && !Std::is_tuple_v<Std::remove_cvref_t<ValueTypes>>);

template <typename ValueType>
struct debug_tuple_format {
    using type = debug_tuple_unsupported;
};

template <typename... ValueTypes>
struct debug_tuple_format<Std::tuple<ValueTypes...>> {
    using type =
        Std::conditional_t<are_debug_scalar_values_v<ValueTypes...>,
                           typename debug_flat_tuple_format<sizeof...(ValueTypes)>::type, debug_tuple_unsupported>;
};

template <typename Value00Type, typename Value01Type, typename Value10Type, typename Value11Type>
struct debug_tuple_format<Std::tuple<Std::tuple<Value00Type, Value01Type>, Std::tuple<Value10Type, Value11Type>>> {
    using type = Std::conditional_t<are_debug_scalar_values_v<Value00Type, Value01Type, Value10Type, Value11Type>,
                                    debug_tuple_nested_2x2, debug_tuple_unsupported>;
};

template <typename LeadingType, typename Value0Type, typename Value1Type>
struct debug_tuple_format<Std::tuple<LeadingType, Std::tuple<Value0Type, Value1Type>>> {
    using type = Std::conditional_t<are_debug_scalar_values_v<LeadingType, Value0Type, Value1Type>,
                                    debug_tuple_leading_scalar_flat_2, debug_tuple_unsupported>;
};

template <typename LeadingType, typename Value00Type, typename Value01Type, typename Value10Type, typename Value11Type>
struct debug_tuple_format<
    Std::tuple<LeadingType, Std::tuple<Std::tuple<Value00Type, Value01Type>, Std::tuple<Value10Type, Value11Type>>>> {
    using type =
        Std::conditional_t<are_debug_scalar_values_v<LeadingType, Value00Type, Value01Type, Value10Type, Value11Type>,
                           debug_tuple_leading_scalar_nested_2x2, debug_tuple_unsupported>;
};

template <typename ValueType>
using debug_tuple_format_t = typename debug_tuple_format<Std::remove_cvref_t<ValueType>>::type;

struct debug_slice_same_format_tag {};
struct debug_slice_flat_source_tag {};
struct debug_slice_nested_source_tag {};
struct debug_slice_rank_expanded_tag {};
struct debug_slice_unsupported_format_tag {};

enum class slice_error_kind { source_shape, slice_shape, coord };
enum class tensor_layout_error_kind { shape, stride };

template <typename FormatType>
constexpr bool is_debug_slice_same_format_v =
    is_one_of_attr_v<FormatType, debug_tuple_flat_1, debug_tuple_flat_2, debug_tuple_flat_3, debug_tuple_flat_4,
                     debug_tuple_flat_5, debug_tuple_flat_6, debug_tuple_nested_2x2, debug_tuple_leading_scalar_flat_2,
                     debug_tuple_leading_scalar_nested_2x2>;

template <typename SourceFormatType, typename CoordFormatType, typename SliceFormatType>
struct debug_slice_format_category {
    static constexpr bool is_same_format = Std::is_same_v<SourceFormatType, CoordFormatType>
                                           && Std::is_same_v<CoordFormatType, SliceFormatType>
                                           && is_debug_slice_same_format_v<SourceFormatType>;
    static constexpr bool is_flat_source =
        Std::is_same_v<SourceFormatType, debug_tuple_flat_2> && Std::is_same_v<CoordFormatType, SliceFormatType>
        && is_one_of_attr_v<CoordFormatType, debug_tuple_nested_2x2, debug_tuple_leading_scalar_flat_2,
                            debug_tuple_leading_scalar_nested_2x2>;
    static constexpr bool is_nested_source =
        Std::is_same_v<SourceFormatType, debug_tuple_nested_2x2> && Std::is_same_v<CoordFormatType, SliceFormatType>
        && is_one_of_attr_v<CoordFormatType, debug_tuple_flat_2, debug_tuple_leading_scalar_flat_2,
                            debug_tuple_leading_scalar_nested_2x2>;
    static constexpr bool is_rank_expanded =
        (Std::is_same_v<SourceFormatType, debug_tuple_flat_3> && Std::is_same_v<CoordFormatType, debug_tuple_flat_4>
         && Std::is_same_v<SliceFormatType, debug_tuple_flat_4>)
        || (Std::is_same_v<SourceFormatType, debug_tuple_flat_4> && Std::is_same_v<CoordFormatType, debug_tuple_flat_5>
            && Std::is_same_v<SliceFormatType, debug_tuple_flat_5>)
        || (Std::is_same_v<SourceFormatType, debug_tuple_flat_5> && Std::is_same_v<CoordFormatType, debug_tuple_flat_6>
            && Std::is_same_v<SliceFormatType, debug_tuple_flat_6>);

    using type = Std::conditional_t<
        is_same_format, debug_slice_same_format_tag,
        Std::conditional_t<is_flat_source, debug_slice_flat_source_tag,
                           Std::conditional_t<is_nested_source, debug_slice_nested_source_tag,
                                              Std::conditional_t<is_rank_expanded, debug_slice_rank_expanded_tag,
                                                                 debug_slice_unsupported_format_tag>>>>;
};

template <typename SourceFormatType, typename CoordFormatType, typename SliceFormatType>
using debug_slice_format_category_t =
    typename debug_slice_format_category<SourceFormatType, CoordFormatType, SliceFormatType>::type;

template <typename ShapeType>
struct is_debug_ext_shape : Std::false_type {};

template <typename Value00Type, typename Value01Type, typename Value10Type, typename Value11Type>
struct is_debug_ext_shape<Std::tuple<Std::tuple<Value00Type, Value01Type>, Std::tuple<Value10Type, Value11Type>>>
    : Std::bool_constant<are_debug_scalar_values_v<Value00Type, Value01Type, Value10Type, Value11Type>
                         && Std::is_constant<1, Std::remove_cvref_t<Value00Type>>::value
                         && Std::is_constant<1, Std::remove_cvref_t<Value10Type>>::value> {};

template <typename ShapeType>
constexpr bool is_debug_ext_shape_v = is_debug_ext_shape<Std::remove_cvref_t<ShapeType>>::value;

template <typename ShapeType>
struct is_debug_batch_ext_shape : Std::false_type {};

template <typename BatchType, typename Value00Type, typename Value01Type, typename Value10Type, typename Value11Type>
struct is_debug_batch_ext_shape<
    Std::tuple<BatchType, Std::tuple<Std::tuple<Value00Type, Value01Type>, Std::tuple<Value10Type, Value11Type>>>>
    : Std::bool_constant<are_debug_scalar_values_v<BatchType, Value00Type, Value01Type, Value10Type, Value11Type>
                         && Std::is_constant<1, Std::remove_cvref_t<Value00Type>>::value
                         && Std::is_constant<1, Std::remove_cvref_t<Value10Type>>::value> {};

template <typename ShapeType>
constexpr bool is_debug_batch_ext_shape_v = is_debug_batch_ext_shape<Std::remove_cvref_t<ShapeType>>::value;

template <typename PatternType>
__aicore__ inline constexpr auto get_debug_layout_pattern_name()
{
    if constexpr (Std::is_same_v<PatternType, zn_layout_ptn>) {
        return "zn_layout_ptn";
    } else if constexpr (Std::is_same_v<PatternType, zz_layout_ptn>) {
        return "zz_layout_ptn";
    } else if constexpr (Std::is_same_v<PatternType, nn_layout_ptn>) {
        return "nn_layout_ptn";
    } else if constexpr (Std::is_same_v<PatternType, nz_layout_ptn>) {
        return "nz_layout_ptn";
    } else if constexpr (Std::is_same_v<PatternType, nd_layout_ptn>) {
        return "nd_layout_ptn";
    } else if constexpr (Std::is_same_v<PatternType, dn_layout_ptn>) {
        return "dn_layout_ptn";
    } else if constexpr (Std::is_same_v<PatternType, nd_ext_layout_ptn>) {
        return "nd_ext_layout_ptn";
    } else if constexpr (Std::is_same_v<PatternType, dn_ext_layout_ptn>) {
        return "dn_ext_layout_ptn";
    } else if constexpr (Std::is_same_v<PatternType, scalea_nd_layout_ptn>) {
        return "scalea_nd_layout_ptn";
    } else if constexpr (Std::is_same_v<PatternType, scalea_dn_layout_ptn>) {
        return "scalea_dn_layout_ptn";
    } else if constexpr (Std::is_same_v<PatternType, scaleb_nd_layout_ptn>) {
        return "scaleb_nd_layout_ptn";
    } else if constexpr (Std::is_same_v<PatternType, scaleb_dn_layout_ptn>) {
        return "scaleb_dn_layout_ptn";
    } else if constexpr (Std::is_same_v<PatternType, nchw_layout_ptn>) {
        return "nchw_layout_ptn";
    } else if constexpr (Std::is_same_v<PatternType, nhwc_layout_ptn>) {
        return "nhwc_layout_ptn";
    } else if constexpr (Std::is_same_v<PatternType, nc1hwc0_layout_ptn>) {
        return "nc1hwc0_layout_ptn";
    } else if constexpr (Std::is_same_v<PatternType, ncdhw_layout_ptn>) {
        return "ncdhw_layout_ptn";
    } else if constexpr (Std::is_same_v<PatternType, ndc1hwc0_layout_ptn>) {
        return "ndc1hwc0_layout_ptn";
    } else if constexpr (Std::is_same_v<PatternType, Std::ignore_t>) {
        return "Unspecified";
    } else {
        return "Unknown";
    }
}

template <typename LayoutType>
struct debug_layout_view {
    using layout_type = Std::remove_cvref_t<LayoutType>;
    using pattern_type = get_layout_pattern<layout_type>;
    using shape_type = Std::remove_cvref_t<decltype(Std::declval<const layout_type&>().shape())>;
    using shape_format_type = debug_tuple_format_t<shape_type>;

    static constexpr bool is_flat_matrix_pattern = is_one_of_attr_v<pattern_type, nd_layout_ptn, dn_layout_ptn>;
    static constexpr bool is_ext_matrix_pattern = is_one_of_attr_v<pattern_type, nd_ext_layout_ptn, dn_ext_layout_ptn,
                                                                   scalea_nd_layout_ptn, scaleb_dn_layout_ptn>;
    static constexpr bool is_fractal_matrix_pattern =
        is_one_of_attr_v<pattern_type, nz_layout_ptn, zn_layout_ptn, zz_layout_ptn, nn_layout_ptn, scalea_dn_layout_ptn,
                         scaleb_nd_layout_ptn>;
    static constexpr bool is_conv_4d_pattern = is_one_of_attr_v<pattern_type, nchw_layout_ptn, nhwc_layout_ptn>;
    static constexpr bool is_conv_5d_pattern = is_one_of_attr_v<pattern_type, nc1hwc0_layout_ptn, ncdhw_layout_ptn>;
    static constexpr bool is_conv_6d_pattern = Std::is_same_v<pattern_type, ndc1hwc0_layout_ptn>;

    static constexpr bool has_batch =
        (is_flat_matrix_pattern && Std::is_same_v<shape_format_type, debug_tuple_leading_scalar_flat_2>)
        || (is_ext_matrix_pattern && is_debug_batch_ext_shape_v<shape_type>)
        || (is_fractal_matrix_pattern && Std::is_same_v<shape_format_type, debug_tuple_leading_scalar_nested_2x2>)
        || (is_conv_4d_pattern && Std::is_same_v<shape_format_type, debug_tuple_flat_4>)
        || (is_conv_5d_pattern && Std::is_same_v<shape_format_type, debug_tuple_flat_5>)
        || (is_conv_6d_pattern && Std::is_same_v<shape_format_type, debug_tuple_flat_6>);

    __aicore__ inline static constexpr auto pattern_name()
    {
        return get_debug_layout_pattern_name<pattern_type>();
    }

    __aicore__ inline static constexpr decltype(auto) batch(const layout_type& layout)
    {
        return get<0>(layout.shape());
    }

    __aicore__ inline static constexpr decltype(auto) batch_stride(const layout_type& layout)
    {
        return get<0>(layout.stride());
    }

    __aicore__ inline static constexpr auto shape(const layout_type& layout)
    {
        if constexpr (is_flat_matrix_pattern && Std::is_same_v<shape_format_type, debug_tuple_leading_scalar_flat_2>) {
            return get<1>(layout.shape());
        } else if constexpr (is_ext_matrix_pattern && is_debug_batch_ext_shape_v<shape_type>) {
            return make_shape(get<1, 0, 1>(layout.shape()), get<1, 1, 1>(layout.shape()));
        } else if constexpr (is_ext_matrix_pattern && is_debug_ext_shape_v<shape_type>) {
            return make_shape(get<0, 1>(layout.shape()), get<1, 1>(layout.shape()));
        } else if constexpr (is_fractal_matrix_pattern
                             && Std::is_same_v<shape_format_type, debug_tuple_leading_scalar_nested_2x2>) {
            return get<1>(layout.shape());
        } else if constexpr (is_conv_4d_pattern && Std::is_same_v<shape_format_type, debug_tuple_flat_4>) {
            return make_shape(get<1>(layout.shape()), get<2>(layout.shape()), get<3>(layout.shape()));
        } else if constexpr (is_conv_5d_pattern && Std::is_same_v<shape_format_type, debug_tuple_flat_5>) {
            return make_shape(get<1>(layout.shape()), get<2>(layout.shape()), get<3>(layout.shape()),
                              get<4>(layout.shape()));
        } else if constexpr (is_conv_6d_pattern && Std::is_same_v<shape_format_type, debug_tuple_flat_6>) {
            return make_shape(get<1>(layout.shape()), get<2>(layout.shape()), get<3>(layout.shape()),
                              get<4>(layout.shape()), get<5>(layout.shape()));
        } else {
            return layout.shape();
        }
    }

    __aicore__ inline static constexpr auto stride(const layout_type& layout)
    {
        if constexpr (is_flat_matrix_pattern && Std::is_same_v<shape_format_type, debug_tuple_leading_scalar_flat_2>) {
            return get<1>(layout.stride());
        } else if constexpr (is_ext_matrix_pattern && is_debug_batch_ext_shape_v<shape_type>) {
            return make_stride(get<1, 0, 1>(layout.stride()), get<1, 1, 1>(layout.stride()));
        } else if constexpr (is_ext_matrix_pattern && is_debug_ext_shape_v<shape_type>) {
            return make_stride(get<0, 1>(layout.stride()), get<1, 1>(layout.stride()));
        } else if constexpr (is_fractal_matrix_pattern
                             && Std::is_same_v<shape_format_type, debug_tuple_leading_scalar_nested_2x2>) {
            return get<1>(layout.stride());
        } else if constexpr (is_conv_4d_pattern && Std::is_same_v<shape_format_type, debug_tuple_flat_4>) {
            return make_stride(get<1>(layout.stride()), get<2>(layout.stride()), get<3>(layout.stride()));
        } else if constexpr (is_conv_5d_pattern && Std::is_same_v<shape_format_type, debug_tuple_flat_5>) {
            return make_stride(get<1>(layout.stride()), get<2>(layout.stride()), get<3>(layout.stride()),
                               get<4>(layout.stride()));
        } else if constexpr (is_conv_6d_pattern && Std::is_same_v<shape_format_type, debug_tuple_flat_6>) {
            return make_stride(get<1>(layout.stride()), get<2>(layout.stride()), get<3>(layout.stride()),
                               get<4>(layout.stride()), get<5>(layout.stride()));
        } else {
            return layout.stride();
        }
    }
};

template <tensor_layout_error_kind kind>
struct tensor_layout_error_traits;

template <>
struct tensor_layout_error_traits<tensor_layout_error_kind::shape> {
    __aicore__ inline static constexpr auto value_name()
    {
        return "shape";
    }
    __aicore__ inline static constexpr auto field_name()
    {
        return "Shape";
    }
    __aicore__ inline static constexpr auto value_description()
    {
        return "shape sizes";
    }
    __aicore__ inline static constexpr auto requirement()
    {
        return "greater than 0";
    }

    template <typename LayoutType>
    __aicore__ inline static constexpr decltype(auto) batch(const LayoutType& layout)
    {
        return debug_layout_view<LayoutType>::batch(layout);
    }

    template <typename LayoutType>
    __aicore__ inline static constexpr auto value(const LayoutType& layout)
    {
        return debug_layout_view<LayoutType>::shape(layout);
    }
};

template <>
struct tensor_layout_error_traits<tensor_layout_error_kind::stride> {
    __aicore__ inline static constexpr auto value_name()
    {
        return "stride";
    }
    __aicore__ inline static constexpr auto field_name()
    {
        return "Stride";
    }
    __aicore__ inline static constexpr auto value_description()
    {
        return "stride values";
    }
    __aicore__ inline static constexpr auto requirement()
    {
        return "non-negative";
    }

    template <typename LayoutType>
    __aicore__ inline static constexpr decltype(auto) batch(const LayoutType& layout)
    {
        return debug_layout_view<LayoutType>::batch_stride(layout);
    }

    template <typename LayoutType>
    __aicore__ inline static constexpr auto value(const LayoutType& layout)
    {
        return debug_layout_view<LayoutType>::stride(layout);
    }
};

template <typename InfoType, bool is_layout = is_layout_v<InfoType>>
struct debug_slice_info_view {
    static constexpr bool has_layout = false;
    static constexpr bool has_batch = false;

    __aicore__ inline static constexpr decltype(auto) shape(const InfoType& info)
    {
        return info;
    }
};

template <typename InfoType>
struct debug_slice_info_view<InfoType, true> {
    using layout_view_type = debug_layout_view<InfoType>;

    static constexpr bool has_layout = true;
    static constexpr bool has_batch = layout_view_type::has_batch;

    __aicore__ inline static constexpr auto pattern_name()
    {
        return layout_view_type::pattern_name();
    }

    __aicore__ inline static constexpr decltype(auto) batch(const InfoType& info)
    {
        return layout_view_type::batch(info);
    }

    __aicore__ inline static constexpr auto shape(const InfoType& info)
    {
        return layout_view_type::shape(info);
    }
};

template <size_t index, size_t tuple_index = 0, typename ValueType>
__aicore__ inline constexpr decltype(auto) get_debug_tuple_leaf(const ValueType& value)
{
    using value_type = Std::remove_cvref_t<ValueType>;
    static_assert(index < nesting_depth_v<value_type>, "The debug tuple leaf index must be in range.");
    if constexpr (Std::is_tuple_v<value_type>) {
        using element_type = typename Std::tuple_element<tuple_index, value_type>::type;
        if constexpr (index < nesting_depth_v<element_type>) {
            return get_debug_tuple_leaf<index, 0>(Std::get<tuple_index>(value));
        } else {
            return get_debug_tuple_leaf<index - nesting_depth_v<element_type>, tuple_index + 1>(value);
        }
    } else {
        static_assert(index == 0, "The debug scalar leaf index must be 0.");
        return value;
    }
}

} // namespace te
} // namespace asc
#endif // ASCENDC_DEBUG

#endif // IMPL_TENSOR_API_UTILS_NPU_DEBUG_UTILS_H

#if defined(TENSOR_API_DEBUG_UTILS_OWNS_INTERNAL_HEADER_ACCESS)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef TENSOR_API_DEBUG_UTILS_OWNS_INTERNAL_HEADER_ACCESS
#endif
