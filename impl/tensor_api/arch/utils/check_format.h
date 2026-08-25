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
    "impl/tensor_api/arch/utils/check_format.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file check_format.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_UTILS_CHECK_FORMAT_H
#define IMPL_TENSOR_API_ARCH_UTILS_CHECK_FORMAT_H

#include "impl/tensor_api/utils/utils_impl.h"
#include "impl/tensor_api/tensor/pointer_pattern.h"
#include "impl/tensor_api/tensor/tensor_impl.h"
#include "impl/tensor_api/arch/utils/is_format.h"

namespace AscendC {
namespace Te {
struct CheckNzLayoutPattern {
    template <typename T, typename TraitType>
    __aicore__ inline static constexpr void Check()
    {
        constexpr auto C0_ELEMENT = TraitType::C0_ELEMENT;
        using ShapeRow0 = typename GetNDimType<T, AttrInfo::Shape, AttrInfo::Row, 0>::type;
        using ShapeColumn0 = typename GetNDimType<T, AttrInfo::Shape, AttrInfo::Column, 0>::type;
        static_assert(
            Std::is_same_v<ShapeRow0, Std::Int<FRACTAL_FIXED>>, "Layout->Shape->Row->ZeroDim must be Int<16>!");
        static_assert(
            Std::is_same_v<ShapeColumn0, Std::Int<C0_ELEMENT>>,
            "Layout->Shape->Column->ZeroDim is different from C0_ELEMENT!");

        using StrideRow0 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Row, 0>::type;
        using StrideColumn0 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Column, 0>::type;
        using StrideRow1 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Row, 1>::type;
        static_assert(
            Std::is_same_v<StrideRow0, Std::Int<C0_ELEMENT>>,
            "Layout->Stride->Row->ZeroDim is different from C0_ELEMENT!");
        static_assert(Std::is_same_v<StrideColumn0, _1>, "Layout->Stride->Column->ZeroDim must be Int<1>!");
        static_assert(
            Std::is_same_v<StrideRow1, Std::Int<C0_ELEMENT * FRACTAL_FIXED>>,
            "Layout->Stride->Row->OneDim is different from C0_ELEMENT * FRACTAL_FIXED!");
    }
};

struct CheckNDLayoutPattern {
    template <typename T, typename TraitType>
    __aicore__ inline static constexpr void Check()
    {
        using StrideColumn0 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Column, 0>::type;
        static_assert(Std::is_same_v<StrideColumn0, _1>, "Layout->Stride->Column must be Int<1>!");
    }
};

struct CheckDNLayoutPattern {
    template <typename T, typename TraitType>
    __aicore__ inline static constexpr void Check()
    {
        using StrideRow0 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Row, 0>::type;
        static_assert(Std::is_same_v<StrideRow0, _1>, "Src->Layout->Stride->Row must be Int<1>!");
    }
};

struct CheckNDExtLayoutPattern {
    template <typename T, typename TraitType>
    __aicore__ inline static constexpr void Check()
    {
        using ShapeRow0 = typename GetNDimType<T, AttrInfo::Shape, AttrInfo::Row, 0>::type;
        using ShapeColumn0 = typename GetNDimType<T, AttrInfo::Shape, AttrInfo::Column, 0>::type;
        static_assert(Std::is_same_v<ShapeRow0, _1>, "Layout->Shape->Row->ZeroDim must be 1!");
        static_assert(Std::is_same_v<ShapeColumn0, _1>, "Layout->Shape->Column->ZeroDim must be 1!");

        using StrideRow0 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Row, 0>::type;
        using StrideColumn0 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Column, 0>::type;
        using StrideColumn1 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Column, 1>::type;
        static_assert(Std::is_same_v<StrideRow0, _0>, "Layout->Stride->Row->ZeroDim must be 0!");
        static_assert(Std::is_same_v<StrideColumn0, _0>, "Layout->Stride->Column->ZeroDim must be 0!");
        static_assert(Std::is_same_v<StrideColumn1, _1>, "Layout->Stride->Column->OneDim must be 1!");
    }
};

struct CheckDNExtLayoutPattern {
    template <typename T, typename TraitType>
    __aicore__ inline static constexpr void Check()
    {
        using ShapeRow0 = typename GetNDimType<T, AttrInfo::Shape, AttrInfo::Row, 0>::type;
        using ShapeColumn0 = typename GetNDimType<T, AttrInfo::Shape, AttrInfo::Column, 0>::type;
        static_assert(Std::is_same_v<ShapeRow0, _1>, "Src->Layout->Shape->Row->ZeroDim must be 1!");
        static_assert(Std::is_same_v<ShapeColumn0, _1>, "Src->Layout->Shape->Column->ZeroDim must be 1!");

        using StrideRow0 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Row, 0>::type;
        using StrideRow1 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Row, 1>::type;
        using StrideColumn0 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Column, 0>::type;
        static_assert(Std::is_same_v<StrideRow0, _0>, "Src->Layout->Stride->Row->ZeroDim must be 0!");
        static_assert(Std::is_same_v<StrideRow1, _1>, "Src->Layout->Stride->Row->OneDim must be 1!");
        static_assert(Std::is_same_v<StrideColumn0, _0>, "Src->Layout->Stride->Column->ZeroDim must be 0!");
    }
};

struct CheckNnLayoutPattern {
    template <typename T, typename TraitType>
    __aicore__ inline static constexpr void Check()
    {
        using ShapeRow0 = typename GetNDimType<T, AttrInfo::Shape, AttrInfo::Row, 0>::type;
        using ShapeColumn0 = typename GetNDimType<T, AttrInfo::Shape, AttrInfo::Column, 0>::type;
        static_assert(Std::is_same_v<ShapeRow0, _2>, "Src->Layout->Shape->Row->ZeroDim must be Int<2>!");
        static_assert(Std::is_same_v<ShapeColumn0, _16>, "Src->Layout->Shape->Column->ZeroDim must be Int<16>!");

        using StrideRow0 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Row, 0>::type;
        using StrideRow1 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Row, 1>::type;
        using StrideColumn0 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Column, 0>::type;
        static_assert(Std::is_same_v<StrideRow0, _1>, "Src->Layout->Stride->Row->ZeroDim must be Int<1>!");
        static_assert(Std::is_same_v<StrideRow1, _32>, "Src->Layout->Stride->Row->OneDim must be Int<32>!");
        static_assert(Std::is_same_v<StrideColumn0, _2>, "Src->Layout->Stride->Column->ZeroDim must be Int<2>!");
    }
};

struct CheckZzLayoutPattern {
    template <typename T, typename TraitType>
    __aicore__ inline static constexpr void Check()
    {
        constexpr auto C0_ELEMENT = TraitType::C0_ELEMENT;
        using ShapeRow0 = typename GetNDimType<T, AttrInfo::Shape, AttrInfo::Row, 0>::type;
        using ShapeColumn0 = typename GetNDimType<T, AttrInfo::Shape, AttrInfo::Column, 0>::type;
        static_assert(
            Std::is_same_v<ShapeColumn0, Std::Int<C0_ELEMENT>>,
            "Layout->Shape->Column->ZeroDim is different from C0_ELEMENT!");
        static_assert(
            Std::is_same_v<ShapeRow0, Std::Int<FRACTAL_FIXED>>, "Layout->Shape->Row->ZeroDim must be Int<16>!");

        using StrideRow0 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Row, 0>::type;
        using StrideColumn0 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Column, 0>::type;
        static_assert(Std::is_same_v<StrideColumn0, _1>, "Layout->Stride->Column->ZeroDim must be Int<1>!");
        static_assert(
            Std::is_same_v<StrideRow0, Std::Int<C0_ELEMENT>>,
            "Layout->Stride->Row->ZeroDim is different from C0_ELEMENT!");
    }
};

struct CheckZnLayoutPattern {
    template <typename T, typename TraitType>
    __aicore__ inline static constexpr void Check()
    {
        constexpr auto C0_ELEMENT = TraitType::C0_ELEMENT;
        using ShapeRow0 = typename GetNDimType<T, AttrInfo::Shape, AttrInfo::Row, 0>::type;
        using ShapeColumn0 = typename GetNDimType<T, AttrInfo::Shape, AttrInfo::Column, 0>::type;
        static_assert(
            Std::is_same_v<ShapeColumn0, Std::Int<FRACTAL_FIXED>>, "Layout->Shape->Column->ZeroDim must be Int<16>!");
        static_assert(
            Std::is_same_v<ShapeRow0, Std::Int<C0_ELEMENT>>,
            "Layout->Shape->Row->ZeroDim is different from C0_ELEMENT!");

        using StrideRow0 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Row, 0>::type;
        using StrideColumn0 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Column, 0>::type;
        static_assert(
            Std::is_same_v<StrideColumn0, Std::Int<C0_ELEMENT>>,
            "Layout->Stride->Column->ZeroDim is different from C0_ELEMENT!");
        static_assert(Std::is_same_v<StrideRow0, _1>, "Layout->Stride->Row->ZeroDim must be Int<1>!");
    }
};

struct CheckScaleANDLayoutPattern {
    template <typename T, typename TraitType>
    __aicore__ inline static constexpr void Check()
    {
        using ShapeRow0 = typename GetNDimType<T, AttrInfo::Shape, AttrInfo::Row, 0>::type;
        using ShapeColumn0 = typename GetNDimType<T, AttrInfo::Shape, AttrInfo::Column, 0>::type;
        static_assert(Std::is_same_v<ShapeRow0, _1>, "Layout->Shape->Row->ZeroDim must be Int<1>!");
        static_assert(Std::is_same_v<ShapeColumn0, _1>, "Layout->Shape->Column->ZeroDim must be Int<1>!");

        using StrideRow0 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Row, 0>::type;
        using StrideColumn0 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Column, 0>::type;
        using StrideColumn1 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Column, 1>::type;
        static_assert(Std::is_same_v<StrideRow0, _0>, "Layout->Stride->Row->ZeroDim must be Int<0>!");
        static_assert(Std::is_same_v<StrideColumn0, _0>, "Layout->Stride->Column->ZeroDim must be Int<0>!");
        static_assert(Std::is_same_v<StrideColumn1, _1>, "Layout->Stride->Column->OneDim must be Int<1>!");
    }
};

struct CheckScaleADNLayoutPattern {
    template <typename T, typename TraitType>
    __aicore__ inline static constexpr void Check()
    {
        using ShapeRow0 = typename GetNDimType<T, AttrInfo::Shape, AttrInfo::Row, 0>::type;
        using ShapeColumn0 = typename GetNDimType<T, AttrInfo::Shape, AttrInfo::Column, 0>::type;
        static_assert(Std::is_same_v<ShapeRow0, _1>, "Layout->Shape->Row->ZeroDim must be Int<1>!");
        static_assert(Std::is_same_v<ShapeColumn0, _2>, "Layout->Shape->Column->ZeroDim must be Int<2>!");

        using StrideRow0 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Row, 0>::type;
        using StrideRow1 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Row, 1>::type;
        using StrideColumn0 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Column, 0>::type;
        static_assert(Std::is_same_v<StrideRow0, _0>, "Layout->Stride->Row->ZeroDim must be Int<0>!");
        static_assert(Std::is_same_v<StrideRow1, _2>, "Layout->Stride->Row->OneDim must be Int<2>!");
        static_assert(Std::is_same_v<StrideColumn0, _1>, "Layout->Stride->Column->ZeroDim must be Int<1>!");
    }
};

struct CheckScaleBNDLayoutPattern {
    template <typename T, typename TraitType>
    __aicore__ inline static constexpr void Check()
    {
        using ShapeRow0 = typename GetNDimType<T, AttrInfo::Shape, AttrInfo::Row, 0>::type;
        using ShapeColumn0 = typename GetNDimType<T, AttrInfo::Shape, AttrInfo::Column, 0>::type;
        static_assert(Std::is_same_v<ShapeRow0, _2>, "Layout->Shape->Row->ZeroDim must be Int<2>!");
        static_assert(Std::is_same_v<ShapeColumn0, _1>, "Layout->Shape->Column->ZeroDim must be Int<1>!");

        using StrideRow0 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Row, 0>::type;
        using StrideColumn0 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Column, 0>::type;
        using StrideColumn1 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Column, 1>::type;
        static_assert(Std::is_same_v<StrideRow0, _1>, "Layout->Stride->Row->ZeroDim must be Int<1>!");
        static_assert(Std::is_same_v<StrideColumn0, _0>, "Layout->Stride->Column->ZeroDim must be Int<0>!");
        static_assert(Std::is_same_v<StrideColumn1, _2>, "Layout->Stride->Column->OneDim must be Int<2>!");
    }
};

struct CheckScaleBDNLayoutPattern {
    template <typename T, typename TraitType>
    __aicore__ inline static constexpr void Check()
    {
        using ShapeRow0 = typename GetNDimType<T, AttrInfo::Shape, AttrInfo::Row, 0>::type;
        using ShapeColumn0 = typename GetNDimType<T, AttrInfo::Shape, AttrInfo::Column, 0>::type;
        static_assert(Std::is_same_v<ShapeRow0, _1>, "Layout->Shape->Row->ZeroDim must be Int<1>!");
        static_assert(Std::is_same_v<ShapeColumn0, _1>, "Layout->Shape->Column->ZeroDim must be Int<1>!");

        using StrideRow0 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Row, 0>::type;
        using StrideRow1 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Row, 1>::type;
        using StrideColumn0 = typename GetNDimType<T, AttrInfo::Stride, AttrInfo::Column, 0>::type;
        static_assert(Std::is_same_v<StrideRow0, _0>, "Layout->Stride->Row->ZeroDim must be Int<0>!");
        static_assert(Std::is_same_v<StrideRow1, _1>, "Layout->Stride->Row->OneDim must be Int<1>!");
        static_assert(Std::is_same_v<StrideColumn0, _0>, "Layout->Stride->Column->ZeroDim must be Int<0>!");
    }
};

using LayoutPatternCheckSet = TupleMap<
    Std::tuple<ZNLayoutPtn, CheckZnLayoutPattern>, Std::tuple<ZZLayoutPtn, CheckZzLayoutPattern>,
    Std::tuple<NNLayoutPtn, CheckNnLayoutPattern>, Std::tuple<NZLayoutPtn, CheckNzLayoutPattern>,
    Std::tuple<NDLayoutPtn, CheckNDLayoutPattern>, Std::tuple<DNLayoutPtn, CheckDNLayoutPattern>,
    Std::tuple<NDExtLayoutPtn, CheckNDExtLayoutPattern>, Std::tuple<DNExtLayoutPtn, CheckDNExtLayoutPattern>,
    Std::tuple<ScaleANDLayoutPtn, CheckScaleANDLayoutPattern>,
    Std::tuple<ScaleADNLayoutPtn, CheckScaleADNLayoutPattern>,
    Std::tuple<ScaleBNDLayoutPtn, CheckScaleBNDLayoutPattern>,
    Std::tuple<ScaleBDNLayoutPtn, CheckScaleBDNLayoutPattern>>;

template <typename T>
__aicore__ inline void CheckOneLayoutPattern()
{
    using Layout = typename T::layoutType;
    using LayoutPattern = GetLayoutPattern<Layout>;
    using TraitType = GetLayoutTrait<Layout>;
    using PatternCheck = typename LayoutPatternCheckSet::template Get<LayoutPattern>;
    static_assert(!Std::is_same_v<PatternCheck, Std::ignore_t>, "Unsupported layout pattern.");
    PatternCheck::template Check<T, TraitType>();
}

template <typename... Args>
__aicore__ inline void CheckLayoutPattern()
{
    (CheckOneLayoutPattern<Args>(), ...);
}

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_UTILS_CHECK_FORMAT_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
