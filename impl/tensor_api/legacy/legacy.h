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
 * \file legacy.h
 * \brief PascalCase wrapper functions for backward compatibility
 */

#ifndef IMPL_TENSOR_API_LEGACY_LEGACY_H
#define IMPL_TENSOR_API_LEGACY_LEGACY_H

#include "impl/tensor_api/legacy/legacy_atom.h"

namespace asc {
namespace te {

template <typename T>
using GetMemLocation = get_mem_location<T>;

template <typename LayoutPattern, typename TraitType = layout_trait_default<>>
using FrameLayoutFormat = frame_layout_format<LayoutPattern, TraitType>;

template <typename T, typename C0 = Std::Int<C0_ELEMENT<T>>>
using LayoutTrait = layout_trait<T, C0>;

using ArchVersion = arch_version;

template <typename TensorType, typename TargetLayoutPtn>
inline constexpr bool IsSatisfiedPtnFormatV = is_satisfied_ptn_format_v<TensorType, TargetLayoutPtn>;

template <typename T>
inline constexpr bool IsAttrTensorV = is_attr_tensor_v<T>;

template <typename LayoutType>
__aicore__ inline constexpr auto GetTotalColumnShape(const LayoutType& layout)
{
    return get_total_column_shape(layout);
}

template <typename LayoutType>
__aicore__ inline constexpr auto GetTotalRowShape(const LayoutType& layout)
{
    return get_total_row_shape(layout);
}

template <typename T>
__aicore__ inline void SetMTE2NzPara(const T& para)
{
    set_mte2_nz_para(para);
}

template <typename T, typename... Ts>
__aicore__ inline constexpr Shape<T, Ts...> MakeShape(const T& value, const Ts&... values)
{
    return make_shape(value, values...);
}

template <typename T, typename... Ts>
__aicore__ inline constexpr Stride<T, Ts...> MakeStride(const T& value, const Ts&... values)
{
    return make_stride(value, values...);
}

template <typename T, typename... Ts>
__aicore__ inline constexpr Coord<T, Ts...> MakeCoord(const T& value, const Ts&... values)
{
    return make_coord(value, values...);
}

template <typename T, typename U>
__aicore__ inline constexpr auto MakeLayout(const T& shape, const U& stride)
{
    return make_layout(shape, stride);
}

template <typename ShapeType>
__aicore__ inline constexpr auto MakeLayout(const ShapeType& shape)
{
    return make_layout(shape);
}

template <size_t... Is, typename LayoutType, typename = Std::enable_if_t<is_layout_v<LayoutType>>>
__aicore__ inline constexpr auto GetShape(const LayoutType& layout)
{
    return get_shape<Is...>(layout);
}

template <size_t... Is, typename LayoutType, typename = Std::enable_if_t<is_layout_v<LayoutType>>>
__aicore__ inline constexpr auto GetShape(LayoutType& layout)
{
    return get_shape<Is...>(layout);
}

template <size_t... Is, typename LayoutType, typename = Std::enable_if_t<is_layout_v<LayoutType>>>
__aicore__ inline constexpr auto GetStride(const LayoutType& layout)
{
    return get_stride<Is...>(layout);
}

template <size_t... Is, typename LayoutType, typename = Std::enable_if_t<is_layout_v<LayoutType>>>
__aicore__ inline constexpr auto GetStride(LayoutType& layout)
{
    return get_stride<Is...>(layout);
}

template <size_t... Is, typename LayoutType, typename = Std::enable_if_t<is_layout_v<LayoutType>>>
__aicore__ inline constexpr auto Coshape(const LayoutType& layout)
{
    return coshape<Is...>(layout);
}

template <size_t... Is, typename LayoutType, typename = Std::enable_if_t<is_layout_v<LayoutType>>>
__aicore__ inline constexpr auto Cosize(const LayoutType& layout)
{
    return cosize<Is...>(layout);
}

template <size_t... Is, typename LayoutType, typename = Std::enable_if_t<is_layout_v<LayoutType>>>
__aicore__ inline constexpr auto Rank(const LayoutType& layout)
{
    return rank<Is...>(layout);
}

template <size_t... Is, typename LayoutType, typename = Std::enable_if_t<is_layout_v<LayoutType>>>
__aicore__ inline constexpr auto Select(const LayoutType& layout)
{
    return select<Is...>(layout);
}

template <size_t I, typename Tuple, typename = Std::enable_if_t<Std::is_tuple_v<Std::remove_cvref_t<Tuple>>>>
__aicore__ inline constexpr auto Get(Tuple&& tuple)
{
    return get<I>(tuple);
}

template <size_t I0, size_t I1, size_t... Is, typename Tuple,
          typename = Std::enable_if_t<Std::is_tuple_v<Std::remove_cvref_t<Tuple>>>>
__aicore__ inline constexpr auto Get(Tuple&& tuple)
{
    return get<I0, I1, Is...>(tuple);
}

template <typename Tuple, typename = Std::enable_if_t<Std::is_tuple_v<Std::remove_cvref_t<Tuple>>>>
__aicore__ inline constexpr auto Get(Tuple&& tuple)
{
    return get(tuple);
}

template <size_t... Is, typename LayoutType, typename = Std::enable_if_t<is_layout_v<LayoutType>>>
__aicore__ inline constexpr auto Get(const LayoutType& layout)
{
    return get<Is...>(layout);
}

template <size_t... Is, typename LayoutType, typename = Std::enable_if_t<is_layout_v<LayoutType>>>
__aicore__ inline constexpr auto Size(const LayoutType& layout)
{
    return size<Is...>(layout);
}

template <size_t... Is, typename LayoutType, typename = Std::enable_if_t<is_layout_v<LayoutType>>>
__aicore__ inline constexpr auto Capacity(const LayoutType& layout)
{
    return capacity<Is...>(layout);
}

template <typename Tensor, typename CoordType, typename Info>
__aicore__ inline constexpr decltype(auto) Slice(Tensor&& tensor, const CoordType& coord, const Info& info)
{
    return slice(tensor, coord, info);
}

template <typename T, typename U, typename S>
__aicore__ inline constexpr auto Crd2Idx(const T& coord, const U& shape, const S& stride)
{
    return crd2idx(coord, shape, stride);
}

template <typename T, typename LayoutType, typename = Std::enable_if_t<is_layout_v<LayoutType>>>
__aicore__ inline constexpr auto Crd2Idx(const T& coord, const LayoutType& layout)
{
    return crd2idx(coord, layout);
}

template <typename LayoutPattern, typename TraitType = Std::ignore_t, typename... Args>
__aicore__ inline constexpr decltype(auto) MakeFrameLayout(const Args&... args)
{
    return make_frame_layout<LayoutPattern, TraitType>(args...);
}

template <typename LayoutPattern, size_t C0Element, typename... Args>
__aicore__ inline constexpr decltype(auto) MakeFrameLayout(const Args&... args)
{
    return make_frame_layout<LayoutPattern, C0Element>(args...);
}

template <typename LayoutPattern, typename TraitType, typename ShapeType, typename StrideType>
__aicore__ inline constexpr auto MakePatternLayout(const ShapeType& shape, const StrideType& stride)
{
    return make_pattern_layout<LayoutPattern, TraitType>(shape, stride);
}

template <typename PtrPattern, typename Iterator>
__aicore__ inline auto MakeLocationMemPtr(Iterator iterator)
{
    return make_location_mem_ptr<PtrPattern>(iterator);
}

template <typename PtrPattern, typename DataType, typename Addr, enable_make_ptr_by_trait<PtrPattern, Addr> = 0>
__aicore__ inline auto MakeMemPtr(Addr address)
{
    return make_mem_ptr<PtrPattern, DataType>(address);
}

template <typename PtrPattern, typename Iterator, enable_make_hardware_ptr<PtrPattern, Iterator> = 0>
__aicore__ inline constexpr auto MakeMemPtr(Iterator iterator)
{
    return make_mem_ptr<PtrPattern>(iterator);
}

template <typename Iterator, enable_make_ptr_by_iter<Iterator> = 0>
__aicore__ inline constexpr auto MakeMemPtr(Iterator iterator)
{
    return make_mem_ptr(iterator);
}

template <typename Iterator, typename... Args>
__aicore__ inline constexpr auto MakeTensor(const Iterator& iterator, const Args&... args)
{
    return make_tensor(iterator, args...);
}

template <size_t... SqueezeDims, typename T,
          typename = Std::enable_if_t<(is_layout_v<T> || is_attr_tensor_v<T>) && (sizeof...(SqueezeDims) > 0)>>
__aicore__ inline constexpr auto Squeeze(const T& value)
{
    return squeeze<SqueezeDims...>(value);
}

template <typename Pattern, typename T,
          typename = Std::enable_if_t<(is_layout_v<T> || is_attr_tensor_v<T>)
                                      && Std::is_tuple_v<Std::remove_cvref_t<Pattern>>>>
__aicore__ inline constexpr auto Squeeze(const T& value, const Pattern& pattern)
{
    return squeeze(value, pattern);
}

// Mmad / MakeMmad: PascalCase 接口
template <typename... Args, typename DstTensor, typename FmTensor, typename FilterTensor>
__aicore__ inline void Mmad(const MmadAtom<MmadTraits<Args...>>& atom, const DstTensor& dst, const FmTensor& fm,
                            const FilterTensor& filter)
{
    atom.call(dst, fm, filter);
}

template <typename... Args, typename DstTensor, typename FmTensor, typename FilterTensor, typename BiasTensor,
          Std::enable_if_t<is_attr_tensor_v<BiasTensor>, int> = 0>
__aicore__ inline void Mmad(const MmadAtom<MmadTraits<Args...>>& atom, const DstTensor& dst, const FmTensor& fm,
                            const FilterTensor& filter, const BiasTensor& bias)
{
    atom.call(dst, fm, filter, bias);
}

template <typename MmadOperationType>
__aicore__ inline constexpr auto MakeMmad(const MmadOperationType& operation)
{
    return MmadAtom<MmadOperationType>{};
}

template <typename MmadOperationType, typename MmadTraitType>
__aicore__ inline constexpr auto MakeMmad(const MmadOperationType& operation, const MmadTraitType& trait)
{
    return MmadAtom<MmadTraits<MmadOperationType, MmadTraitType>>{};
}

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_LEGACY_LEGACY_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_H
#endif
