/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if defined(__NPU_COMPILER_INTERNAL_PURE_SIMT__)
#error "layout.h cannot be used with compile flag --enable-simt enabled."
#endif

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

namespace AscendC {
namespace Te {

// layout construction
template <typename LayoutPattern, typename TraitType, typename ShapeType, typename StrideType>
__aicore__ inline constexpr auto MakePatternLayout(const ShapeType& shape, const StrideType& stride);

template <typename... Ts>
__aicore__ inline constexpr Shape<Ts...> MakeShape(const Ts&... t);

template <typename... Ts>
__aicore__ inline constexpr Stride<Ts...> MakeStride(const Ts&... t);

template <typename... Ts>
__aicore__ inline constexpr Tile<Ts...> MakeTile(const Ts&... t);

template <typename... Ts>
__aicore__ inline constexpr Coord<Ts...> MakeCoord(const Ts&... t);

template <typename T, typename U>
__aicore__ inline constexpr auto MakeLayout(const T& shape, const U& stride);

template <typename T>
__aicore__ inline constexpr auto MakeLayout(const T& shape);

template <typename T, typename Shape, typename Stride>
__aicore__ inline constexpr auto Crd2Idx(const T& coord, const Shape& shape, const Stride& stride);

template <size_t I, typename Tuple, typename Enable>
__aicore__ inline constexpr auto Get(Tuple&& t);

template <size_t I0, size_t I1, size_t... Is, typename Tuple, typename Enable>
__aicore__ inline constexpr auto Get(Tuple&& t);

template <typename Tuple, typename Enable>
__aicore__ inline constexpr auto Get(Tuple&& t);

template <size_t... Is, typename LayoutType, typename Enable>
__aicore__ inline constexpr auto Cosize(const LayoutType& layout);

template <size_t... Is, typename LayoutType, typename Enable>
__aicore__ inline constexpr auto Rank(const LayoutType& layout);

template <size_t... Is, typename LayoutType, typename Enable>
__aicore__ inline constexpr auto Select(const LayoutType& layout);

template <size_t... Is, typename LayoutType, typename Enable>
__aicore__ inline constexpr auto Get(const LayoutType& layout);

template <size_t... Is, typename LayoutType, typename Enable>
__aicore__ inline constexpr auto Size(const LayoutType& layout);

template <size_t... Is, typename LayoutType, typename Enable>
__aicore__ inline constexpr auto Capacity(const LayoutType& layout);

template <typename Tensor, typename Coord, typename Info>
__aicore__ inline constexpr decltype(auto) Slice(Tensor&& tensor, const Coord& coord, const Info& info);

template <typename LayoutPattern, typename TraitType, typename... Args>
__aicore__ inline constexpr decltype(auto) MakeFrameLayout(const Args&... args);

template <typename LayoutPattern, size_t C0Element, typename... Args>
__aicore__ inline constexpr decltype(auto) MakeFrameLayout(const Args&... args);

template <size_t... SqueezeDims, typename T, typename Enable>
__aicore__ inline constexpr auto Squeeze(const T& x);

template <typename Pattern, typename T, typename Enable>
__aicore__ inline constexpr auto Squeeze(const T& x, const Pattern& pattern);

} // namespace Te
} // namespace AscendC

#endif // INCLUDE_TENSOR_API_TENSOR_LAYOUT_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_H
#endif
