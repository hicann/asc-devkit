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
    "impl/tensor_api/tensor/layout_pattern.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
* \file layout_pattern.h
* \brief
*/
#ifndef IMPL_TENSOR_API_TENSOR_LAYOUT_PATTERN_H
#define IMPL_TENSOR_API_TENSOR_LAYOUT_PATTERN_H

#include "impl/tensor_api/tensor/layout_method.h"

namespace AscendC {
namespace Te {

struct ZNLayoutPtn {};
struct ZZLayoutPtn {};
struct NNLayoutPtn {};
struct NZLayoutPtn {};
struct NDLayoutPtn {};
struct DNLayoutPtn {};
struct NDExtLayoutPtn {};
struct DNExtLayoutPtn {};
struct ScaleANDLayoutPtn {};
struct ScaleADNLayoutPtn {};
struct ScaleBNDLayoutPtn {};
struct ScaleBDNLayoutPtn {};

template <typename LayoutPattern, typename TraitType, typename ShapeType, typename StrideType>
__aicore__ inline constexpr auto MakePatternLayout(const ShapeType& shape, const StrideType& stride)
{
    using LayoutT = Layout<ShapeType, StrideType, Std::tuple<LayoutPattern, TraitType>>;
    return LayoutT(shape, stride);
} 

template <typename LayoutPattern, typename TraitType, typename Batch, typename LayoutType>
__aicore__ inline constexpr auto MakeBatchPatternLayout(const Batch& batch, const LayoutType& layout)
{
    return MakePatternLayout<LayoutPattern, TraitType>(
        MakeShape(batch, layout.Shape()), MakeStride(layout.Capacity(), layout.Stride()));
}

struct MakeNzFrameLayout {
    template <typename TraitType, typename T, typename U>
    __aicore__ inline static auto Make(T row, U column) {
        constexpr auto c0Ele = TraitType::C0_ELEMENT;
        auto shape = MakeShape(MakeShape(Std::Int<FRACTAL_FIXED>{}, Std::ceil_division(row, FRACTAL_FIXED)),
                               MakeShape(c0Ele, Std::ceil_division(column, c0Ele)));
        auto stride = MakeStride(MakeStride(c0Ele, c0Ele * Std::Int<FRACTAL_FIXED>{}),
                                 MakeStride(_1{}, c0Ele * Std::ceil_align(row, FRACTAL_FIXED)));
        using LayoutT = Layout<decltype(shape), decltype(stride), Std::tuple<NZLayoutPtn, TraitType>>;
        return LayoutT(shape, stride);
    }

    template <typename TraitType, typename Batch, typename T, typename U>
    __aicore__ inline static auto Make(Batch batch, T row, U column) {
        return MakeBatchPatternLayout<NZLayoutPtn, TraitType>(batch, Make<TraitType>(row, column));
    }
};

struct MakeNDExtFrameLayout {
    template <typename TraitType, typename T, typename U>
    __aicore__ inline static auto Make(T row, U column) {
        auto shape = MakeShape(MakeShape(_1{}, row), MakeShape(_1{}, column));
        auto stride = MakeStride(MakeStride(_0{}, column), MakeStride(_0{}, _1{}));
        using LayoutT = Layout<decltype(shape), decltype(stride), Std::tuple<NDExtLayoutPtn, TraitType>>;
        return LayoutT(shape, stride);
    }

    template <typename TraitType, typename Batch, typename T, typename U>
    __aicore__ inline static auto Make(Batch batch, T row, U column) {
        return MakeBatchPatternLayout<NDExtLayoutPtn, TraitType>(batch, Make<TraitType>(row, column));
    }
};

struct MakeNDFrameLayout {
    template <typename TraitType, typename T, typename U>
    __aicore__ inline static auto Make(T row, U column) {
        auto shape = MakeShape(row, column);
        auto stride = MakeStride(column, _1{});
        using LayoutT = Layout<decltype(shape), decltype(stride), Std::tuple<NDLayoutPtn, TraitType>>;
        return LayoutT(shape, stride);
    }

    template <typename TraitType, typename Batch, typename T, typename U>
    __aicore__ inline static auto Make(Batch batch, T row, U column) {
        return MakeBatchPatternLayout<NDLayoutPtn, TraitType>(batch, Make<TraitType>(row, column));
    }
};

struct MakeZnFrameLayout {
    template <typename TraitType, typename T, typename U>
    __aicore__ inline static auto Make(T row, U column) {
        constexpr auto c0Ele = TraitType::C0_ELEMENT;
        auto shape = MakeShape(MakeShape(c0Ele, Std::ceil_division(row, c0Ele)),
                               MakeShape(Std::Int<FRACTAL_FIXED>{}, Std::ceil_division(column, FRACTAL_FIXED)));
        auto stride = MakeStride(MakeStride(_1{}, c0Ele * Std::ceil_align(column, FRACTAL_FIXED)),
                                 MakeStride(c0Ele, c0Ele * Std::Int<FRACTAL_FIXED>{}));
        using LayoutT = Layout<decltype(shape), decltype(stride), Std::tuple<ZNLayoutPtn, TraitType>>;
        return LayoutT(shape, stride);
    }

    template <typename TraitType, typename Batch, typename T, typename U>
    __aicore__ inline static auto Make(Batch batch, T row, U column) {
        return MakeBatchPatternLayout<ZNLayoutPtn, TraitType>(batch, Make<TraitType>(row, column));
    }
};

struct MakeDNFrameLayout {
    template <typename TraitType, typename T, typename U>
    __aicore__ inline static auto Make(T row, U column) {
        auto shape = MakeShape(row, column);
        auto stride = MakeStride(_1{}, row);
        using LayoutT = Layout<decltype(shape), decltype(stride), Std::tuple<DNLayoutPtn, TraitType>>;
        return LayoutT(shape, stride);
    }

    template <typename TraitType, typename Batch, typename T, typename U>
    __aicore__ inline static auto Make(Batch batch, T row, U column) {
        return MakeBatchPatternLayout<DNLayoutPtn, TraitType>(batch, Make<TraitType>(row, column));
    }
};

struct MakeDNExtFrameLayout {
    template <typename TraitType, typename T, typename U>
    __aicore__ inline static auto Make(T row, U column) {
        auto shape = MakeShape(MakeShape(_1{}, row), MakeShape(_1{}, column));
        auto stride = MakeStride(MakeStride(_0{}, _1{}), MakeStride(_0{}, row));
        using LayoutT = Layout<decltype(shape), decltype(stride), Std::tuple<DNExtLayoutPtn, TraitType>>;
        return LayoutT(shape, stride);
    }

    template <typename TraitType, typename Batch, typename T, typename U>
    __aicore__ inline static auto Make(Batch batch, T row, U column) {
        return MakeBatchPatternLayout<DNExtLayoutPtn, TraitType>(batch, Make<TraitType>(row, column));
    }
};

struct MakeZzFrameLayout {
    template <typename TraitType, typename T, typename U>
    __aicore__ inline static auto Make(T row, U column) {
        constexpr auto c0Ele = TraitType::C0_ELEMENT;
        auto shape = MakeShape(MakeShape(Std::Int<FRACTAL_FIXED>{}, Std::ceil_division(row, FRACTAL_FIXED)),
                               MakeShape(c0Ele, Std::ceil_division(column, c0Ele)));
        auto stride = MakeStride(MakeStride(c0Ele, FRACTAL_FIXED * Std::ceil_align(column, c0Ele)),
                                 MakeStride(_1{}, c0Ele * Std::Int<FRACTAL_FIXED>{}));
        using LayoutT = Layout<decltype(shape), decltype(stride), Std::tuple<ZZLayoutPtn, TraitType>>;
        return LayoutT(shape, stride);
    }

    template <typename TraitType, typename Batch, typename T, typename U>
    __aicore__ inline static auto Make(Batch batch, T row, U column) {
        return MakeBatchPatternLayout<ZZLayoutPtn, TraitType>(batch, Make<TraitType>(row, column));
    }
};

struct MakeNnFrameLayout {
    template <typename TraitType, typename T, typename U>
    __aicore__ inline static auto Make(T row, U column) {
        constexpr auto c0Ele = TraitType::C0_ELEMENT;
        static_assert(c0Ele == 2, "NnLayoutPtn only supports fp8_e8m0_t and ShapeColumn0 as 2.");
        auto shape = MakeShape(MakeShape(c0Ele, row / c0Ele), MakeShape(Std::Int<FRACTAL_FIXED>{}, Std::ceil_division(column, FRACTAL_FIXED)));
        auto stride = MakeStride(MakeStride(_1{}, c0Ele * Std::Int<FRACTAL_FIXED>{}),
                                 MakeStride(c0Ele, row * FRACTAL_FIXED));
        using LayoutT = Layout<decltype(shape), decltype(stride), Std::tuple<NNLayoutPtn, TraitType>>;
        return LayoutT(shape, stride);
    }

    template <typename TraitType, typename Batch, typename T, typename U>
    __aicore__ inline static auto Make(Batch batch, T row, U column) {
        return MakeBatchPatternLayout<NNLayoutPtn, TraitType>(batch, Make<TraitType>(row, column));
    }
};

struct MakeScaleANDFrameLayout {
    template <typename TraitType, typename T, typename U>
    __aicore__ inline static auto Make(T row, U column) {
        constexpr auto c0Ele = TraitType::C0_ELEMENT;
        static_assert(c0Ele == 2, "ScaleANDLayoutPtn only supports fp8_e8m0_t and ShapeColumn0 as 2.");
        auto shape = MakeShape(MakeShape(_1{}, row), MakeShape(_1{}, column));
        auto stride = MakeStride(MakeStride(_0{}, column), MakeStride(_0{}, _1{}));
        using LayoutT = Layout<decltype(shape), decltype(stride), Std::tuple<ScaleANDLayoutPtn, TraitType>>;
        return LayoutT(shape, stride);
    }

    template <typename TraitType, typename Batch, typename T, typename U>
    __aicore__ inline static auto Make(Batch batch, T row, U column) {
        return MakeBatchPatternLayout<ScaleANDLayoutPtn, TraitType>(batch, Make<TraitType>(row, column));
    }
};

struct MakeScaleADNFrameLayout {
    template <typename TraitType, typename T, typename U>
    __aicore__ inline static auto Make(T row, U column) {
        constexpr auto c0Ele = TraitType::C0_ELEMENT;
        static_assert(c0Ele == 2, "ScaleADNLayoutPtn only supports fp8_e8m0_t and ShapeColumn0 as 2.");
        auto shape = MakeShape(MakeShape(_1{}, row), MakeShape(c0Ele, column / c0Ele));
        auto stride = MakeStride(MakeStride(_0{}, c0Ele),
                                 MakeStride(_1{}, c0Ele * row));
        using LayoutT = Layout<decltype(shape), decltype(stride), Std::tuple<ScaleADNLayoutPtn, TraitType>>;
        return LayoutT(shape, stride);
    }

    template <typename TraitType, typename Batch, typename T, typename U>
    __aicore__ inline static auto Make(Batch batch, T row, U column) {
        return MakeBatchPatternLayout<ScaleADNLayoutPtn, TraitType>(batch, Make<TraitType>(row, column));
    }
};

struct MakeScaleBNDFrameLayout {
    template <typename TraitType, typename T, typename U>
    __aicore__ inline static auto Make(T row, U column) {
        constexpr auto c0Ele = TraitType::C0_ELEMENT;
        static_assert(c0Ele == 2, "ScaleBNDLayoutPtn only supports fp8_e8m0_t and ShapeColumn0 as 2.");
        auto shape = MakeShape(MakeShape(c0Ele, row / c0Ele), MakeShape(_1{}, column));
        auto stride = MakeStride(MakeStride(_1{}, c0Ele * column),
                                 MakeStride(_0{}, c0Ele));
        using LayoutT = Layout<decltype(shape), decltype(stride), Std::tuple<ScaleBNDLayoutPtn, TraitType>>;
        return LayoutT(shape, stride);
    }

    template <typename TraitType, typename Batch, typename T, typename U>
    __aicore__ inline static auto Make(Batch batch, T row, U column) {
        return MakeBatchPatternLayout<ScaleBNDLayoutPtn, TraitType>(batch, Make<TraitType>(row, column));
    }
};

struct MakeScaleBDNFrameLayout {
    template <typename TraitType, typename T, typename U>
    __aicore__ inline static auto Make(T row, U column) {
        auto shape = MakeShape(MakeShape(_1{}, row), MakeShape(_1{}, column));
        auto stride = MakeStride(MakeStride(_0{}, _1{}), MakeStride(_0{}, row));
        using LayoutT = Layout<decltype(shape), decltype(stride), Std::tuple<ScaleBDNLayoutPtn, TraitType>>;
        return LayoutT(shape, stride);
    }

    template <typename TraitType, typename Batch, typename T, typename U>
    __aicore__ inline static auto Make(Batch batch, T row, U column) {
        return MakeBatchPatternLayout<ScaleBDNLayoutPtn, TraitType>(batch, Make<TraitType>(row, column));
    }
};

struct NCHWLayoutPtn {};
struct NHWCLayoutPtn {};
struct NC1HWC0LayoutPtn {};

struct MakeNCHWFrameLayout {
    template <typename TraitType, typename N, typename C, typename H, typename W>
    __aicore__ inline static auto Make(N n, C c, H h, W w) {
        auto shape = MakeShape(n, c, h, w);
        auto stride = MakeStride(c * h * w, h * w, w, _1{});
        using LayoutT = Layout<decltype(shape), decltype(stride), Std::tuple<NCHWLayoutPtn, TraitType>>;
        return LayoutT(shape, stride);
    }
};

struct MakeNHWCFrameLayout {
    template <typename TraitType, typename N, typename H, typename W, typename C>
    __aicore__ inline static auto Make(N n, H h, W w, C c) {
        auto shape = MakeShape(n, h, w, c);
        auto stride = MakeStride(h * w * c, w * c, c, _1{});
        using LayoutT = Layout<decltype(shape), decltype(stride), Std::tuple<NHWCLayoutPtn, TraitType>>;
        return LayoutT(shape, stride);
    }
};

struct MakeNC1HWC0FrameLayout {
    template <typename TraitType, typename N, typename C1, typename H, typename W, typename C0>
    __aicore__ inline static auto Make(N n, C1 c1, H h, W w, C0 c0) {
        auto shape = MakeShape(n, c1, h, w, c0);
        auto stride = MakeStride(c1 * h * w * c0, h * w * c0, w * c0, c0, _1{});
        using LayoutT = Layout<decltype(shape), decltype(stride), Std::tuple<NC1HWC0LayoutPtn, TraitType>>;
        return LayoutT(shape, stride);
    }
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_TENSOR_LAYOUT_PATTERN_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
