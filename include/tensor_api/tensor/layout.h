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
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_H
#endif

/*!
 * \file layout.h
 * \brief
 */
#ifndef INCLUDE_TENSOR_API_TENSOR_LAYOUT_H
#define INCLUDE_TENSOR_API_TENSOR_LAYOUT_H

#include "impl/tensor_api/tensor/layout_impl.h"

namespace asc {
namespace te {

// layout construction
template <typename... Ts>
__aicore__ inline constexpr shape_type<Ts...> make_shape(const Ts&... t);

template <typename... Ts>
__aicore__ inline constexpr stride_type<Ts...> make_stride(const Ts&... t);

template <typename... Ts>
__aicore__ inline constexpr tile_type<Ts...> make_tile(const Ts&... t);

template <typename... Ts>
__aicore__ inline constexpr coord_type<Ts...> make_coord(const Ts&... t);

template <typename T, typename U>
__aicore__ inline constexpr auto make_layout(const T& shape, const U& stride);

template <typename T>
__aicore__ inline constexpr auto make_layout(const T& shape);

template <typename T, typename Shape, typename Stride> 
__aicore__ inline constexpr auto crd2idx(const T& coord, const Shape& shape, const Stride& stride);

template <size_t I, typename Tuple, typename Enable>
__aicore__ inline constexpr auto get(Tuple&& t);

template <size_t I0, size_t I1, size_t... Is, typename Tuple, typename Enable>
__aicore__ inline constexpr auto get(Tuple&& t);

template <typename Tuple, typename Enable>
__aicore__ inline constexpr auto get(Tuple&& t);

template <size_t... Is, typename LayoutType, typename Enable>
__aicore__ inline constexpr auto cosize(const LayoutType& layout);

template <size_t... Is, typename LayoutType, typename Enable>
__aicore__ inline constexpr auto rank(const LayoutType& layout);

template <size_t... Is, typename LayoutType, typename Enable>
__aicore__ inline constexpr auto select(const LayoutType& layout);

template <size_t... Is, typename LayoutType, typename Enable>
__aicore__ inline constexpr auto get(const LayoutType& layout);

template <size_t... Is, typename LayoutType, typename Enable>
__aicore__ inline constexpr auto size(const LayoutType& layout);

template <size_t... Is, typename LayoutType, typename Enable>
__aicore__ inline constexpr auto capacity(const LayoutType& layout);

template <typename Tensor, typename Coord, typename Info>
__aicore__ inline constexpr decltype(auto) slice(Tensor&& tensor, const Coord& coord, const Info& info);

template <typename LayoutPattern, typename TraitType, typename... Args>
__aicore__ inline constexpr decltype(auto) make_frame_layout(const Args&... args);

template <typename LayoutPattern, size_t C0Element, typename... Args>
__aicore__ inline constexpr decltype(auto) make_frame_layout(const Args&... args);

template <size_t... SqueezeDims, typename T, typename Enable>
__aicore__ inline constexpr auto squeeze(const T& x);

template <typename Pattern, typename T, typename Enable>
__aicore__ inline constexpr auto squeeze(const T& x, const Pattern& pattern);

} // namespace te
} // namespace asc

#endif // INCLUDE_TENSOR_API_TENSOR_LAYOUT_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_H
#endif
