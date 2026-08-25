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
#warning \
    "impl/tensor_api/tensor/layout_infer.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file layout_infer.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_TENSOR_LAYOUT_INFER_H
#define IMPL_TENSOR_API_TENSOR_LAYOUT_INFER_H

#include "impl/tensor_api/tensor/layout_frame.h"

namespace AscendC {
namespace Te {

constexpr size_t INFER_NESTED_LAYOUT_ND_EXT_PRIORITY = 0;
constexpr size_t INFER_NESTED_LAYOUT_DN_EXT_PRIORITY = 1;
constexpr size_t INFER_NESTED_LAYOUT_NN_PRIORITY = 2;
constexpr size_t INFER_NESTED_LAYOUT_NZ_PRIORITY = 3;
constexpr size_t INFER_NESTED_LAYOUT_ZZ_PRIORITY = 4;
constexpr size_t INFER_NESTED_LAYOUT_ZN_PRIORITY = 5;
constexpr size_t INFER_NESTED_LAYOUT_SCALE_ADN_PRIORITY = 6;
constexpr size_t INFER_NESTED_LAYOUT_SCALE_BND_PRIORITY = 7;
constexpr size_t INFER_NESTED_LAYOUT_PRIORITY_END = 8;

template <typename RowStride, typename ColumnStride>
struct InferFlatLayoutPatternImpl {
    using type = Std::ignore_t;
};

template <typename RowStride>
struct InferFlatLayoutPatternImpl<RowStride, _1> {
    using type = NDLayoutPtn;
};

template <typename ColumnStride>
struct InferFlatLayoutPatternImpl<_1, ColumnStride> {
    using type = DNLayoutPtn;
};

template <>
struct InferFlatLayoutPatternImpl<_1, _1> {
    using type = NDLayoutPtn;
};

template <typename RowStride, typename ColumnStride>
struct InferFlatLayoutPattern
    : InferFlatLayoutPatternImpl<Std::remove_cvref_t<RowStride>, Std::remove_cvref_t<ColumnStride>> {};

template <
    typename ShapeRow0, typename ShapeRow1, typename ShapeColumn0, typename ShapeColumn1, typename StrideRow0,
    typename StrideRow1, typename StrideColumn0, typename StrideColumn1,
    size_t Priority = INFER_NESTED_LAYOUT_ND_EXT_PRIORITY, typename Enable = void>
struct InferNestedLayoutPatternImpl {
    using type = typename InferNestedLayoutPatternImpl<
        ShapeRow0, ShapeRow1, ShapeColumn0, ShapeColumn1, StrideRow0, StrideRow1, StrideColumn0, StrideColumn1,
        Priority + 1>::type;
};

template <
    typename ShapeRow0, typename ShapeRow1, typename ShapeColumn0, typename ShapeColumn1, typename StrideRow0,
    typename StrideRow1, typename StrideColumn0, typename StrideColumn1>
struct InferNestedLayoutPatternImpl<
    ShapeRow0, ShapeRow1, ShapeColumn0, ShapeColumn1, StrideRow0, StrideRow1, StrideColumn0, StrideColumn1,
    INFER_NESTED_LAYOUT_PRIORITY_END> {
    using type = Std::ignore_t;
};

template <typename ShapeRow1, typename ShapeColumn1, typename StrideRow1>
struct InferNestedLayoutPatternImpl<
    _1, ShapeRow1, _1, ShapeColumn1, _0, StrideRow1, _0, _1, INFER_NESTED_LAYOUT_ND_EXT_PRIORITY> {
    using type = NDExtLayoutPtn;
};

template <typename ShapeRow1, typename ShapeColumn1, typename StrideColumn1>
struct InferNestedLayoutPatternImpl<
    _1, ShapeRow1, _1, ShapeColumn1, _0, _1, _0, StrideColumn1, INFER_NESTED_LAYOUT_DN_EXT_PRIORITY> {
    using type = DNExtLayoutPtn;
};

template <typename ShapeRow1, typename ShapeColumn1, typename StrideColumn1>
struct InferNestedLayoutPatternImpl<
    _2, ShapeRow1, _16, ShapeColumn1, _1, _32, _2, StrideColumn1, INFER_NESTED_LAYOUT_NN_PRIORITY> {
    using type = NNLayoutPtn;
};

template <size_t ShapeRow0, typename ShapeRow1, size_t ShapeColumn0, typename ShapeColumn1, typename StrideColumn1>
struct InferNestedLayoutPatternImpl<
    Std::Int<ShapeRow0>, ShapeRow1, Std::Int<ShapeColumn0>, ShapeColumn1, Std::Int<ShapeColumn0>,
    Std::Int<ShapeRow0 * ShapeColumn0>, _1, StrideColumn1, INFER_NESTED_LAYOUT_NZ_PRIORITY,
    Std::enable_if_t<ShapeRow0 == FRACTAL_FIXED>> {
    using type = NZLayoutPtn;
};

template <size_t ShapeRow0, typename ShapeRow1, size_t ShapeColumn0, typename ShapeColumn1, typename StrideRow1>
struct InferNestedLayoutPatternImpl<
    Std::Int<ShapeRow0>, ShapeRow1, Std::Int<ShapeColumn0>, ShapeColumn1, Std::Int<ShapeColumn0>, StrideRow1, _1,
    Std::Int<ShapeRow0 * ShapeColumn0>, INFER_NESTED_LAYOUT_ZZ_PRIORITY, Std::enable_if_t<ShapeRow0 == FRACTAL_FIXED>> {
    using type = ZZLayoutPtn;
};

template <size_t ShapeRow0, typename ShapeRow1, size_t ShapeColumn0, typename ShapeColumn1, typename StrideRow1>
struct InferNestedLayoutPatternImpl<
    Std::Int<ShapeRow0>, ShapeRow1, Std::Int<ShapeColumn0>, ShapeColumn1, _1, StrideRow1, Std::Int<ShapeRow0>,
    Std::Int<ShapeRow0 * ShapeColumn0>, INFER_NESTED_LAYOUT_ZN_PRIORITY,
    Std::enable_if_t<ShapeColumn0 == FRACTAL_FIXED>> {
    using type = ZNLayoutPtn;
};

template <typename ShapeRow1, typename ShapeColumn1, typename StrideColumn1>
struct InferNestedLayoutPatternImpl<
    _1, ShapeRow1, _2, ShapeColumn1, _0, _2, _1, StrideColumn1, INFER_NESTED_LAYOUT_SCALE_ADN_PRIORITY> {
    using type = ScaleADNLayoutPtn;
};

template <typename ShapeRow1, typename ShapeColumn1, typename StrideRow1>
struct InferNestedLayoutPatternImpl<
    _2, ShapeRow1, _1, ShapeColumn1, _1, StrideRow1, _0, _2, INFER_NESTED_LAYOUT_SCALE_BND_PRIORITY> {
    using type = ScaleBNDLayoutPtn;
};

template <typename ShapeRows, typename ShapeColumns, typename StrideRows, typename StrideColumns>
struct InferNestedLayoutPattern {
    using type = Std::ignore_t;
};

template <
    typename ShapeRow0, typename ShapeRow1, typename ShapeColumn0, typename ShapeColumn1, typename StrideRow0,
    typename StrideRow1, typename StrideColumn0, typename StrideColumn1>
struct InferNestedLayoutPattern<
    Shape<ShapeRow0, ShapeRow1>, Shape<ShapeColumn0, ShapeColumn1>, Stride<StrideRow0, StrideRow1>,
    Stride<StrideColumn0, StrideColumn1>>
    : InferNestedLayoutPatternImpl<
          Std::remove_cvref_t<ShapeRow0>, Std::remove_cvref_t<ShapeRow1>, Std::remove_cvref_t<ShapeColumn0>,
          Std::remove_cvref_t<ShapeColumn1>, Std::remove_cvref_t<StrideRow0>, Std::remove_cvref_t<StrideRow1>,
          Std::remove_cvref_t<StrideColumn0>, Std::remove_cvref_t<StrideColumn1>> {};

template <
    typename Row, typename Column, typename RowStride, typename ColumnStride,
    bool RowIsTuple = Std::is_tuple_v<Std::remove_cvref_t<Row>>,
    bool ColumnIsTuple = Std::is_tuple_v<Std::remove_cvref_t<Column>>,
    bool RowStrideIsTuple = Std::is_tuple_v<Std::remove_cvref_t<RowStride>>,
    bool ColumnStrideIsTuple = Std::is_tuple_v<Std::remove_cvref_t<ColumnStride>>>
struct InferTwoDimLayoutPattern {
    using type = Std::ignore_t;
};

template <typename Row, typename Column, typename RowStride, typename ColumnStride>
struct InferTwoDimLayoutPattern<Row, Column, RowStride, ColumnStride, false, false, false, false>
    : InferFlatLayoutPattern<RowStride, ColumnStride> {};

template <typename Row, typename Column, typename RowStride, typename ColumnStride>
struct InferTwoDimLayoutPattern<Row, Column, RowStride, ColumnStride, true, true, true, true>
    : InferNestedLayoutPattern<Row, Column, RowStride, ColumnStride> {};

template <typename Batch, typename MatrixShape, typename BatchStride, typename MatrixStride>
struct InferTwoDimLayoutPattern<Batch, MatrixShape, BatchStride, MatrixStride, false, true, false, true>
    : InferTwoDimLayoutPattern<
          typename Std::tuple_element<0, MatrixShape>::type, typename Std::tuple_element<1, MatrixShape>::type,
          typename Std::tuple_element<0, MatrixStride>::type, typename Std::tuple_element<1, MatrixStride>::type> {};

template <typename ShapeType, typename StrideType>
struct InferLayoutPattern {
    using type = Std::ignore_t;
};

template <typename Row, typename Column, typename RowStride, typename ColumnStride>
struct InferLayoutPattern<Shape<Row, Column>, Stride<RowStride, ColumnStride>>
    : InferTwoDimLayoutPattern<Row, Column, RowStride, ColumnStride> {};

template <typename Pattern, typename C0 = _1>
struct BuildInferredLayoutTrait {
    using type =
        Std::conditional_t<Std::is_same_v<Pattern, Std::ignore_t>, Std::ignore_t, LayoutTrait<Std::ignore_t, C0>>;
};

template <typename ShapeType, typename StrideType>
struct InferLayoutTrait {
    using type = Std::ignore_t;
};

template <typename Pattern, typename ShapeRow0, typename ShapeColumn0>
struct InferLayoutC0Type {
    using type = _1;
};

template <typename ShapeRow0, typename ShapeColumn0>
struct InferLayoutC0Type<NZLayoutPtn, ShapeRow0, ShapeColumn0> {
    using type = ShapeColumn0;
};

template <typename ShapeRow0, typename ShapeColumn0>
struct InferLayoutC0Type<ZZLayoutPtn, ShapeRow0, ShapeColumn0> {
    using type = ShapeColumn0;
};

template <typename ShapeRow0, typename ShapeColumn0>
struct InferLayoutC0Type<ZNLayoutPtn, ShapeRow0, ShapeColumn0> {
    using type = ShapeRow0;
};

template <typename ShapeRow0, typename ShapeColumn0>
struct InferLayoutC0Type<NNLayoutPtn, ShapeRow0, ShapeColumn0> {
    using type = ShapeRow0;
};

template <typename ShapeRow0, typename ShapeColumn0>
struct InferLayoutC0Type<ScaleBNDLayoutPtn, ShapeRow0, ShapeColumn0> {
    using type = ShapeRow0;
};

template <typename ShapeRow0, typename ShapeColumn0>
struct InferLayoutC0Type<ScaleADNLayoutPtn, ShapeRow0, ShapeColumn0> {
    using type = ShapeColumn0;
};

template <typename Row, typename Column, typename RowStride, typename ColumnStride>
struct InferLayoutTrait<Shape<Row, Column>, Stride<RowStride, ColumnStride>> {
private:
    using PatternType = typename InferLayoutPattern<Shape<Row, Column>, Stride<RowStride, ColumnStride>>::type;

public:
    using type = typename BuildInferredLayoutTrait<PatternType>::type;
};

template <
    typename ShapeRow0, typename ShapeRow1, typename ShapeColumn0, typename ShapeColumn1, typename StrideRow0,
    typename StrideRow1, typename StrideColumn0, typename StrideColumn1>
struct InferLayoutTrait<
    Shape<Shape<ShapeRow0, ShapeRow1>, Shape<ShapeColumn0, ShapeColumn1>>,
    Stride<Stride<StrideRow0, StrideRow1>, Stride<StrideColumn0, StrideColumn1>>> {
private:
    using LayoutShape = Shape<Shape<ShapeRow0, ShapeRow1>, Shape<ShapeColumn0, ShapeColumn1>>;
    using LayoutStride = Stride<Stride<StrideRow0, StrideRow1>, Stride<StrideColumn0, StrideColumn1>>;
    using PatternType = typename InferLayoutPattern<LayoutShape, LayoutStride>::type;
    using C0Type = typename InferLayoutC0Type<PatternType, ShapeRow0, ShapeColumn0>::type;

public:
    using type = typename BuildInferredLayoutTrait<PatternType, C0Type>::type;
};

template <
    typename Batch, typename ShapeRow0, typename ShapeRow1, typename ShapeColumn0, typename ShapeColumn1,
    typename BatchStride, typename StrideRow0, typename StrideRow1, typename StrideColumn0, typename StrideColumn1>
struct InferLayoutTrait<
    Shape<Batch, Shape<Shape<ShapeRow0, ShapeRow1>, Shape<ShapeColumn0, ShapeColumn1>>>,
    Stride<BatchStride, Stride<Stride<StrideRow0, StrideRow1>, Stride<StrideColumn0, StrideColumn1>>>>
    : InferLayoutTrait<
          Shape<Shape<ShapeRow0, ShapeRow1>, Shape<ShapeColumn0, ShapeColumn1>>,
          Stride<Stride<StrideRow0, StrideRow1>, Stride<StrideColumn0, StrideColumn1>>> {};

template <typename T, typename U>
struct GetLayoutInfo<Layout<T, U, Std::ignore_t>> {
    using type = Std::tuple<typename InferLayoutPattern<T, U>::type, typename InferLayoutTrait<T, U>::type>;
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_TENSOR_LAYOUT_INFER_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
