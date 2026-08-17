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
    "impl/tensor_api/algorithm/copy_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file copy_impl.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ALGORITHM_COPY_IMPL_H
#define IMPL_TENSOR_API_ALGORITHM_COPY_IMPL_H

#include "impl/tensor_api/atom/copy_atom_impl.h"

namespace asc {
namespace te {

template <typename QuantParam>
constexpr bool is_quant_scalar_v =
    !is_attr_tensor_v<QuantParam> && Std::is_convertible_v<Std::remove_cvref_t<QuantParam>, uint64_t>;

template <typename QuantParam>
constexpr bool is_copy_quant_param_v = is_quant_scalar_v<QuantParam> || is_attr_tensor_v<QuantParam>;

template <typename Coord>
constexpr bool is_valid_coord_v = Std::is_tuple_v<Std::remove_cvref_t<Coord>> ||
    Std::is_same_v<Std::remove_cvref_t<Coord>, zero_coord_type>;

template <typename CopyShape>
constexpr bool is_valid_shape_v = Std::is_tuple_v<Std::remove_cvref_t<CopyShape>>;

template <typename Coord, typename CopyShape>
constexpr bool is_copy_coord_shape_compatible_v =
    Std::is_same_v<Std::remove_cvref_t<Coord>, zero_coord_type> || is_same_structure_v<Coord, CopyShape>;

template <typename Pattern>
constexpr bool is_conv_layout_pattern_v = Std::is_same_v<Pattern, nchw_layout_ptn> ||
    Std::is_same_v<Pattern, nhwc_layout_ptn> || Std::is_same_v<Pattern, nc1hwc0_layout_ptn> ||
    Std::is_same_v<Pattern, ncdhw_layout_ptn> || Std::is_same_v<Pattern, ndc1hwc0_layout_ptn>;

template <typename SrcTensor, typename CopyShape>
struct is_copy_shape_compatible_with_src_layout {
private:
    using layout_type = typename Std::remove_cvref_t<SrcTensor>::layout_type;
    using src_shape_type = Std::remove_cvref_t<decltype(Std::declval<layout_type>().shape())>;
    using shape_type = Std::remove_cvref_t<CopyShape>;
    using pattern_type = get_layout_pattern<layout_type>;

    static constexpr size_t src_depth = nesting_depth_v<src_shape_type>;
    static constexpr size_t shape_depth = nesting_depth_v<shape_type>;
    static constexpr size_t src_rank = Std::tuple_size_v<src_shape_type>;
    static constexpr size_t shape_rank = Std::tuple_size_v<shape_type>;
    static constexpr size_t batch_num = src_rank - 1;

    static constexpr bool is_same_structure = is_same_structure_v<src_shape_type, shape_type>;
    static constexpr bool is_logical_matrix = !is_conv_layout_pattern_v<pattern_type> && src_rank == TWO_DIM_DATA &&
        src_depth == FOUR_DIM_DATA && shape_depth == TWO_DIM_DATA;
    static constexpr bool is_logical_flat_batch = !is_conv_layout_pattern_v<pattern_type> && src_rank == shape_rank &&
        src_rank >= TWO_DIM_DATA && src_depth == batch_num + FOUR_DIM_DATA &&
        shape_depth == batch_num + TWO_DIM_DATA;

public:
    static constexpr bool value = is_same_structure || is_logical_matrix || is_logical_flat_batch;
};

template <typename SrcTensor, typename CopyShape>
constexpr bool is_copy_shape_compatible_with_src_layout_v =
    is_copy_shape_compatible_with_src_layout<SrcTensor, CopyShape>::value;

template <typename SrcTensor, typename SrcCoord, typename CopyShape>
__aicore__ inline constexpr void check_copy_params()
{
    static_assert(is_copy_coord_shape_compatible_v<SrcCoord, CopyShape>,
        "Explicit src_coord and shape must have the same tuple structure.");
    static_assert(is_copy_shape_compatible_with_src_layout_v<SrcTensor, CopyShape>,
        "Shape must match the source layout shape, or use a supported logical matrix/batch shape.");
}

template <typename AtomType, typename DstTensor, typename SrcTensor>
__aicore__ inline void copy(const copy_atom<AtomType>& atom, const DstTensor& dst, const SrcTensor& src)
{
    atom.call(dst, src);
}

template <typename AtomType, typename DstTensor, typename SrcTensor, typename QuantParam,
          Std::enable_if_t<is_copy_quant_param_v<QuantParam>, int> = 0>
__aicore__ inline void copy(const copy_atom<AtomType>& atom, const DstTensor& dst, const SrcTensor& src,
                            const QuantParam& quant)
{
    atom.call(dst, src, quant);
}

template <typename AtomType, typename DstTensor, typename SrcTensor, typename DstCoord, typename SrcCoord,
    typename CopyShape, Std::enable_if_t<is_valid_coord_v<DstCoord> && is_valid_coord_v<SrcCoord> &&
        is_valid_shape_v<CopyShape>, int> = 0>
__aicore__ inline void copy(const copy_atom<AtomType>& atom, const DstTensor& dst, const SrcTensor& src,
    const DstCoord& dst_coord, const SrcCoord& src_coord, const CopyShape& copy_shape)
{
    check_copy_params<SrcTensor, SrcCoord, CopyShape>();
    atom.call(dst, src, dst_coord, src_coord, copy_shape);
}

template <typename AtomType, typename DstTensor, typename SrcTensor, typename QuantParam, typename DstCoord,
    typename SrcCoord, typename CopyShape,
    Std::enable_if_t<is_copy_quant_param_v<QuantParam> && is_valid_coord_v<DstCoord> &&
        is_valid_coord_v<SrcCoord> && is_valid_shape_v<CopyShape>, int> = 0>
__aicore__ inline void copy(const copy_atom<AtomType>& atom, const DstTensor& dst, const SrcTensor& src,
    const QuantParam& quant, const DstCoord& dst_coord, const SrcCoord& src_coord, const CopyShape& copy_shape)
{
    check_copy_params<SrcTensor, SrcCoord, CopyShape>();
    atom.call(dst, src, quant, dst_coord, src_coord, copy_shape);
}

template <typename CopyOperationType>
__aicore__ inline constexpr auto make_copy(const CopyOperationType& copy_operation)
{
    return copy_atom<copy_traits<CopyOperationType>>{};
}

template <typename CopyOperationType, typename CopyTraitType>
__aicore__ inline constexpr auto make_copy(const CopyOperationType& copy_operation, const CopyTraitType& copy_trait)
{
    return copy_atom<copy_traits<CopyOperationType, CopyTraitType>>{};
}

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ALGORITHM_COPY_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
