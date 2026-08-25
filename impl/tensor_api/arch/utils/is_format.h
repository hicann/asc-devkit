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
    "impl/tensor_api/arch/utils/is_format.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file is_format.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_UTILS_IS_FORMAT_H
#define IMPL_TENSOR_API_ARCH_UTILS_IS_FORMAT_H

#include "impl/tensor_api/utils/utils_impl.h"
#include "impl/tensor_api/tensor/pointer_pattern.h"
#include "impl/tensor_api/tensor/tensor_impl.h"

namespace AscendC {
namespace Te {

template <typename T, bool IsTuple>
struct ToTupleImpl;

template <typename T>
struct ToTupleImpl<T, true> {
    using type = T;
};

template <typename T>
struct ToTupleImpl<T, false> {
    using type = Std::tuple<T>;
};

template <typename T>
using ToTuple = ToTupleImpl<T, Std::is_tuple_v<T>>;

// Selects the (row, col) shape/stride sub-tuples from a layout's top-level (rowPart, colPart) pair.
// Batched layouts carry a leading Batch axis, so the top-level pair is (Int<B>, matrixTuple): the
// real row/col live one level down, inside the matrix tuple. Non-batched layouts use the pair as-is.
template <typename ShapeRows, typename ShapeCols, typename StrideRows, typename StrideCols, bool IsBatched>
struct SelectRowColTuples {
    using Rows = ShapeRows;
    using Cols = ShapeCols;
    using StrideRowsT = StrideRows;
    using StrideColsT = StrideCols;
};

template <typename ShapeRows, typename ShapeCols, typename StrideRows, typename StrideCols>
struct SelectRowColTuples<ShapeRows, ShapeCols, StrideRows, StrideCols, true> {
    using Rows = typename Std::tuple_element<0, ShapeCols>::type;
    using Cols = typename Std::tuple_element<1, ShapeCols>::type;
    using StrideRowsT = typename Std::tuple_element<0, StrideCols>::type;
    using StrideColsT = typename Std::tuple_element<1, StrideCols>::type;
};

template <typename T>
struct GetTypeFromNDimTrait;

template <
    template <typename> class TensorType, typename hPos, typename Pointer, typename ShapeRows, typename ShapeCols,
    typename StrideRows, typename StrideCols, typename LayoutPattern>
struct GetTypeFromNDimTrait<TensorType<TensorAttribute<
    ViewEngine<HardwareMemPtr<hPos, Pointer>>,
    Layout<Shape<ShapeRows, ShapeCols>, Stride<StrideRows, StrideCols>, LayoutPattern>>>> {
    // A batched layout has a scalar Batch axis as rowPart and the matrix tuple as colPart.
    static constexpr bool IsBatched = !Std::is_tuple_v<ShapeRows> && Std::is_tuple_v<ShapeCols>;
    using Selector = SelectRowColTuples<ShapeRows, ShapeCols, StrideRows, StrideCols, IsBatched>;

    using ShapeRowTuple = typename ToTuple<typename Selector::Rows>::type;
    using ShapeColTuple = typename ToTuple<typename Selector::Cols>::type;
    using StrideRowTuple = typename ToTuple<typename Selector::StrideRowsT>::type;
    using StrideColTuple = typename ToTuple<typename Selector::StrideColsT>::type;

    template <size_t Dim>
    using ShapeRowDim = typename Std::tuple_element<Dim, ShapeRowTuple>::type;

    template <size_t Dim>
    using ShapeColDim = typename Std::tuple_element<Dim, ShapeColTuple>::type;

    template <size_t Dim>
    using StrideRowDim = typename Std::tuple_element<Dim, StrideRowTuple>::type;

    template <size_t Dim>
    using StrideColDim = typename Std::tuple_element<Dim, StrideColTuple>::type;
};

namespace AttrInfo {
struct Shape {};
struct Stride {};
struct Row {};
struct Column {};
}; // namespace AttrInfo

template <typename T, typename info1, typename info2, size_t dim>
struct GetNDimType;

template <typename T, size_t dim>
struct GetNDimType<T, AttrInfo::Shape, AttrInfo::Row, dim> {
    using type = Std::remove_cvref_t<typename GetTypeFromNDimTrait<Std::remove_cvref_t<T>>::template ShapeRowDim<dim>>;
};

template <typename T, size_t dim>
struct GetNDimType<T, AttrInfo::Shape, AttrInfo::Column, dim> {
    using type = Std::remove_cvref_t<typename GetTypeFromNDimTrait<Std::remove_cvref_t<T>>::template ShapeColDim<dim>>;
};

template <typename T, size_t dim>
struct GetNDimType<T, AttrInfo::Stride, AttrInfo::Row, dim> {
    using type = Std::remove_cvref_t<typename GetTypeFromNDimTrait<Std::remove_cvref_t<T>>::template StrideRowDim<dim>>;
};

template <typename T, size_t dim>
struct GetNDimType<T, AttrInfo::Stride, AttrInfo::Column, dim> {
    using type = Std::remove_cvref_t<typename GetTypeFromNDimTrait<Std::remove_cvref_t<T>>::template StrideColDim<dim>>;
};

template <typename TensorType, typename TargetLayoutPtn>
struct IsSatisfiedPtnFormat {
    using LayoutPattern = GetLayoutPattern<typename Std::remove_cvref_t<TensorType>::layoutType>;
    static constexpr bool value = Std::is_same_v<LayoutPattern, TargetLayoutPtn>;
};

template <typename TensorType, typename TargetLayoutPtn>
inline constexpr bool IsSatisfiedPtnFormatV = IsSatisfiedPtnFormat<TensorType, TargetLayoutPtn>::value;
} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_UTILS_IS_FORMAT_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
