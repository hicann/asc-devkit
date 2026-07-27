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
    "impl/tensor_api/tensor/layout_frame.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file layout_frame.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_TENSOR_LAYOUT_FRAME_H
#define IMPL_TENSOR_API_TENSOR_LAYOUT_FRAME_H

#include "impl/tensor_api/tensor/layout_pattern.h"

namespace AscendC {
namespace Te {

using LayoutFormatSet = TupleMap<
    Std::tuple<NZLayoutPtn, MakeNzFrameLayout>, Std::tuple<NDLayoutPtn, MakeNDFrameLayout>,
    Std::tuple<DNLayoutPtn, MakeDNFrameLayout>, Std::tuple<NDExtLayoutPtn, MakeNDExtFrameLayout>,
    Std::tuple<DNExtLayoutPtn, MakeDNExtFrameLayout>, Std::tuple<NNLayoutPtn, MakeNnFrameLayout>,
    Std::tuple<ZZLayoutPtn, MakeZzFrameLayout>, Std::tuple<ZNLayoutPtn, MakeZnFrameLayout>,
    Std::tuple<ScaleANDLayoutPtn, MakeScaleANDFrameLayout>, Std::tuple<ScaleADNLayoutPtn, MakeScaleADNFrameLayout>,
    Std::tuple<ScaleBNDLayoutPtn, MakeScaleBNDFrameLayout>, Std::tuple<ScaleBDNLayoutPtn, MakeScaleBDNFrameLayout>,
    Std::tuple<NCHWLayoutPtn, MakeNCHWFrameLayout>, Std::tuple<NHWCLayoutPtn, MakeNHWCFrameLayout>,
    Std::tuple<NC1HWC0LayoutPtn, MakeNC1HWC0FrameLayout>>;

template <typename T, typename C0>
struct LayoutTrait {
    static constexpr auto C0_ELEMENT = C0{};
};

template <typename T = uint16_t, typename C0 = Std::Int<C0_ELEMENT<T>>>
struct LayoutTraitDefault : LayoutTrait<T, C0> {};

struct LayoutTraitScale : LayoutTraitDefault<fp8_e8m0_t, Std::Int<MX_SCALE_K0 / sizeof(fp8_e8m0_t)>> {};

struct LayoutTraitFP4 : LayoutTraitDefault<fp4x2_e2m1_t, Std::Int<C0_ELEMENT<fp4x2_e2m1_t>>> {};

using FormatTraitSet = TupleMap<
    Std::tuple<NZLayoutPtn, LayoutTraitDefault<>>, Std::tuple<NDLayoutPtn, LayoutTrait<Std::ignore_t, _1>>,
    Std::tuple<DNLayoutPtn, LayoutTrait<Std::ignore_t, _1>>, Std::tuple<NDExtLayoutPtn, LayoutTrait<Std::ignore_t, _1>>,
    Std::tuple<DNExtLayoutPtn, LayoutTrait<Std::ignore_t, _1>>, Std::tuple<NNLayoutPtn, LayoutTrait<Std::ignore_t, _2>>,
    Std::tuple<ZZLayoutPtn, LayoutTraitDefault<>>, Std::tuple<ZNLayoutPtn, LayoutTraitDefault<>>,
    Std::tuple<ScaleANDLayoutPtn, LayoutTrait<Std::ignore_t, _2>>,
    Std::tuple<ScaleADNLayoutPtn, LayoutTrait<Std::ignore_t, _2>>,
    Std::tuple<ScaleBNDLayoutPtn, LayoutTrait<Std::ignore_t, _2>>,
    Std::tuple<ScaleBDNLayoutPtn, LayoutTrait<Std::ignore_t, _2>>,
    Std::tuple<NCHWLayoutPtn, LayoutTrait<Std::ignore_t, _1>>,
    Std::tuple<NHWCLayoutPtn, LayoutTrait<Std::ignore_t, _1>>,
    Std::tuple<NC1HWC0LayoutPtn, LayoutTrait<Std::ignore_t, _1>>>;

template <typename T, typename = void>
struct IsFrameLayoutTrait : Std::false_type {};

template <typename T>
struct IsFrameLayoutTrait<T, void_t<decltype(T::C0_ELEMENT)>> : Std::true_type {};

template <typename T>
constexpr bool IsFrameLayoutTraitV = IsFrameLayoutTrait<T>::value;

template <typename LayoutPattern, typename TraitType>
struct TraitConversion {
private:
    using RawTrait = Std::remove_cvref_t<TraitType>;

    static_assert(
        Std::is_same_v<RawTrait, Std::ignore_t> || IsIntegralConstantV<RawTrait> || IsFrameLayoutTraitV<RawTrait> ||
            IsDataType<RawTrait>,
        "TraitType must be ignore_t, integral constant, frame layout trait, or data type.");

    using FromPattern = typename FormatTraitSet::template Get<LayoutPattern>;
    using FromInt = LayoutTrait<Std::ignore_t, RawTrait>;
    using FromDataType = LayoutTraitDefault<RawTrait>;
    using FromTrait = RawTrait;

    using Converted = typename Std::conditional<
        Std::is_same_v<RawTrait, Std::ignore_t>, FromPattern,
        typename Std::conditional<
            IsIntegralConstantV<RawTrait>, FromInt,
            typename Std::conditional<IsFrameLayoutTraitV<RawTrait>, FromTrait, FromDataType>::type>::type>::type;

public:
    using type = Converted;
};

template <typename LayoutPattern, typename TraitType = Std::ignore_t, typename... Args>
__aicore__ inline constexpr decltype(auto) MakeFrameLayout(const Args&... args)
{
    using Trait = typename TraitConversion<LayoutPattern, TraitType>::type;
    using LayoutMaker = typename LayoutFormatSet::template Get<LayoutPattern>;
    static_assert(!Std::is_same_v<LayoutMaker, Std::ignore_t>, "Unsupported layout pattern.");
    return LayoutMaker::template Make<Trait>(args...);
}

template <typename LayoutPattern, size_t C0Element, typename... Args>
__aicore__ inline constexpr decltype(auto) MakeFrameLayout(const Args&... args)
{
    return MakeFrameLayout<LayoutPattern, Std::Int<C0Element>>(args...);
}

template <typename LayoutPattern, typename TraitType = LayoutTraitDefault<>>
struct FrameLayoutFormat {
    template <typename... Args>
    __aicore__ inline constexpr decltype(auto) operator()(const Args&... args)
    {
        return MakeFrameLayout<LayoutPattern, TraitType>(args...);
    }
};
} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_TENSOR_LAYOUT_FRAME_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
