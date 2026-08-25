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
    "impl/tensor_api/tensor/tensor_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file tensor_impl.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_TENSOR_TENSOR_IMPL_H
#define IMPL_TENSOR_API_TENSOR_TENSOR_IMPL_H

#include "impl/tensor_api/utils/utils_impl.h"
#include "impl/tensor_api/tensor/layout_impl.h"
#include "impl/tensor_api/tensor/engine_impl.h"
#include "impl/tensor_api/tensor/pointer_pattern.h"

namespace AscendC {

template <typename T>
class GlobalTensor;

template <typename T>
class LocalTensor;

template <typename EngineType, typename LayoutType>
struct TensorAttribute {};

namespace Te {

template <typename LocationType, typename EngineType, typename LayoutType>
struct MakeTensorResult {
    using type = typename Std::conditional<
        Std::is_same_v<LocationType, Location::GM>, AscendC::GlobalTensor<TensorAttribute<EngineType, LayoutType>>,
        AscendC::LocalTensor<TensorAttribute<EngineType, LayoutType>>>::type;
};

template <typename EngineType, typename LayoutType>
struct BaseTensor {
    using iterator = typename EngineType::iterator;
    using valueType = typename EngineType::valueType;
    using elementType = typename EngineType::elementType;
    using reference = typename EngineType::reference;

    using engineType = EngineType;
    using layoutType = LayoutType;

    __aicore__ inline BaseTensor() {}
    __aicore__ inline BaseTensor(const EngineType& engine, const LayoutType& layout) : rep(layout, engine) {}

    static constexpr int rank = LayoutType::rank; // tuple size

    __aicore__ inline constexpr decltype(auto) Tensor() const { return *this; }

    __aicore__ inline constexpr decltype(auto) Engine() const { return Std::get<1>(rep); }

    __aicore__ inline constexpr decltype(auto) Engine() { return Std::get<1>(rep); }

    __aicore__ inline constexpr decltype(auto) Layout() const { return Std::get<0>(rep); }

    __aicore__ inline constexpr decltype(auto) Data() const { return Engine().Begin(); }

    __aicore__ inline constexpr decltype(auto) Data() { return Engine().Begin(); }

    __aicore__ inline constexpr decltype(auto) Shape() const { return Layout().Shape(); }

    __aicore__ inline constexpr decltype(auto) Stride() const { return Layout().Stride(); }

    __aicore__ inline constexpr auto Size() const { return Layout().Size(); }

    __aicore__ inline constexpr auto Capacity() const { return Layout().Capacity(); }

    template <typename Coord>
    __aicore__ inline constexpr decltype(auto) operator[](const Coord& coord)
    {
        return Data()[Layout()(coord)];
    }

    template <typename Coord>
    __aicore__ inline constexpr decltype(auto) operator[](const Coord& coord) const
    {
        return Data()[Layout()(coord)];
    }

    template <typename Coord>
    __aicore__ inline constexpr decltype(auto) operator()(const Coord& coord)
    {
        auto sliceEngine = Engine() + Layout()(coord);
        auto coordLayout = MakeCoordLayout(coord, Layout());
        return MakeSubTensor(sliceEngine, coordLayout);
    }

    template <typename Coord>
    __aicore__ inline constexpr decltype(auto) operator()(const Coord& coord) const
    {
        auto sliceEngine = Engine() + Layout()(coord);
        auto coordLayout = MakeCoordLayout(coord, Layout());
        return MakeSubTensor(sliceEngine, coordLayout);
    }

    template <typename Coord0, typename Coord1, typename... Coords>
    __aicore__ inline constexpr decltype(auto) operator()(const Coord0& c0, const Coord1& c1, const Coords&... cs)
    {
        return operator()(MakeCoord(c0, c1, cs...));
    }

    template <typename Coord0, typename Coord1, typename... Coords>
    __aicore__ inline constexpr decltype(auto) operator()(const Coord0& c0, const Coord1& c1, const Coords&... cs) const
    {
        return operator()(MakeCoord(c0, c1, cs...));
    }

    template <typename Coord, typename Info>
    __aicore__ inline constexpr decltype(auto) Slice(const Coord& coord, const Info& info)
    {
        auto sliceEngine = Engine() + Layout()(coord);
        auto coordLayout = MakeSliceLayout(coord, Layout(), info);
        return MakeSubTensor(sliceEngine, coordLayout);
    }

    template <typename Coord, typename Info>
    __aicore__ inline constexpr decltype(auto) Slice(const Coord& coord, const Info& info) const
    {
        auto sliceEngine = Engine() + Layout()(coord);
        auto coordLayout = MakeSliceLayout(coord, Layout(), info);
        return MakeSubTensor(sliceEngine, coordLayout);
    }

private:
    template <typename SliceEngine, typename SliceLayout>
    __aicore__ inline static constexpr decltype(auto) MakeSubTensor(
        const SliceEngine& sliceEngine, const SliceLayout& sliceLayout)
    {
        using Location = GetMemLocation<SliceEngine>;
        using ResultTensor = typename MakeTensorResult<Location, SliceEngine, SliceLayout>::type;
        return ResultTensor{sliceEngine, sliceLayout};
    }

    Std::tuple<layoutType, engineType> rep;
};

template <typename T>
struct IsAttrTensor : Std::false_type {};

template <template <typename> class TensorType, typename Engine, typename Layout>
struct IsAttrTensor<TensorType<TensorAttribute<Engine, Layout>>> : Std::true_type {};

template <typename T>
constexpr bool IsAttrTensorV = IsAttrTensor<Std::remove_cvref_t<T>>::value;

template <typename T>
struct MakeTensorBuilder {
    template <typename Arg0, typename... Args>
    __aicore__ inline constexpr auto operator()(const Arg0& arg0, const Args&... args) const
    {
        using Engine = ViewEngine<Arg0>;
        if constexpr (sizeof...(Args) == 1 && (IsLayoutV<Args> && ...)) {
            using Layout = typename Std::tuple_element<0, Std::tuple<Args...>>::type;
            using Location = GetMemLocation<Engine>;
            using ResultTensor = typename MakeTensorResult<Location, Engine, Layout>::type;
            return ResultTensor{Engine{arg0}, args...};
        } else if constexpr (sizeof...(Args) == 2 && (Std::is_tuple_v<Std::remove_cvref_t<Args>> && ...)) {
            using Layout = decltype(MakeLayout(args...));
            using Location = GetMemLocation<Engine>;
            using ResultTensor = typename MakeTensorResult<Location, Engine, Layout>::type;
            return ResultTensor{Engine{arg0}, MakeLayout(args...)};
        } else {
            static_assert(
                sizeof...(Args) != sizeof...(Args),
                "MakeTensor expected a hardware memory pointer and data structure like Layout or Shape and Stride");
        }
    }
};

template <typename Iterator, typename... Args>
__aicore__ inline constexpr auto MakeTensor(const Iterator& iter, const Args&... args)
{
    static_assert(
        IsHardwareMemPtrV<Iterator>, "MakeTensor expects the first argument to be a memory pointer or iterator");
    return MakeTensorBuilder<Iterator>{}(iter, args...);
}

// Construct a single-batch sub-tensor from a batched tensor by offsetting the engine pointer
// and stripping the leading batch axis from the layout. Layout depth 5 -> 4 (e.g. NZ fractal)
// or 3 -> 2 (e.g. ND row/col). Other depths trigger a compile-time error.
template <typename Tensor>
__aicore__ inline constexpr auto MakeSingleBatchSubTensor(const Tensor& t, uint32_t batchIdx)
{
    using LayoutType = typename Tensor::layoutType;
    static_assert(
        LayoutType::depth == THREE_DIM_DATA || LayoutType::depth == FIVE_DIM_DATA,
        "MakeSingleBatchSubTensor only supports batched layouts of depth 3 or 5.");
    auto layout = t.Layout();
    auto batchStride = Get<0>(layout.Stride());
    auto subEngine = t.Engine() + batchStride * batchIdx;
    auto subLayout = RemoveBatchDim(layout);
    using NewEngine = Std::remove_cvref_t<decltype(subEngine)>;
    using NewLayout = Std::remove_cvref_t<decltype(subLayout)>;
    using Location = GetMemLocation<NewEngine>;
    using NewTensor = typename MakeTensorResult<Location, NewEngine, NewLayout>::type;
    return NewTensor{subEngine, subLayout};
}

// Squeeze (mode 1): drop axes (by index) whose compile-time size is 1; non-one axes are kept (no error).
// Accepts either a Layout (returns a new Layout) or a Tensor (returns a new Tensor over the same engine).
template <
    size_t... SqueezeDims, typename T,
    typename = Std::enable_if_t<(IsLayoutV<T> || IsAttrTensorV<T>) && (sizeof...(SqueezeDims) > 0)>>
__aicore__ inline constexpr auto Squeeze(const T& x)
{
    if constexpr (IsAttrTensorV<T>) {
        auto newLayout = SqueezeLayout<SqueezeDims...>(x.Layout());
        using NewLayout = Std::remove_cvref_t<decltype(newLayout)>;
        using NewEngine = typename T::engineType;
        using Location = GetMemLocation<NewEngine>;
        using NewTensor = typename MakeTensorResult<Location, NewEngine, NewLayout>::type;
        return NewTensor{x.Engine(), newLayout};
    } else {
        static_assert(IsLayoutV<T>, "Squeeze input must be a Layout or Tensor");
        return SqueezeLayout<SqueezeDims...>(x);
    }
}

// Squeeze (mode 2): drop positions marked _1 in an isomorphic pattern tuple, when size is 1.
// Accepts either a Layout or a Tensor.
template <
    typename Pattern, typename T,
    typename = Std::enable_if_t<(IsLayoutV<T> || IsAttrTensorV<T>) && Std::is_tuple_v<Std::remove_cvref_t<Pattern>>>>
__aicore__ inline constexpr auto Squeeze(const T& x, const Pattern& pattern)
{
    if constexpr (IsAttrTensorV<T>) {
        auto newLayout = SqueezeLayout(x.Layout(), pattern);
        using NewLayout = Std::remove_cvref_t<decltype(newLayout)>;
        using NewEngine = typename T::engineType;
        using Location = GetMemLocation<NewEngine>;
        using NewTensor = typename MakeTensorResult<Location, NewEngine, NewLayout>::type;
        return NewTensor{x.Engine(), newLayout};
    } else {
        static_assert(IsLayoutV<T>, "Squeeze input must be a Layout or Tensor");
        return SqueezeLayout(x, pattern);
    }
}

} // namespace Te

template <typename EngineType, typename LayoutType>
struct GlobalTensor<TensorAttribute<EngineType, LayoutType>> : public Te::BaseTensor<EngineType, LayoutType> {
    using TensorApiBase = Te::BaseTensor<EngineType, LayoutType>;

    using TensorApiBase::TensorApiBase;

    __aicore__ inline GlobalTensor() = default;

    __aicore__ inline constexpr void SetL2CacheHint(Te::CacheMode mode) { this->Engine().SetCacheMode(mode); }
};

template <typename EngineType, typename LayoutType>
struct LocalTensor<TensorAttribute<EngineType, LayoutType>> : public Te::BaseTensor<EngineType, LayoutType> {
    using TensorApiBase = Te::BaseTensor<EngineType, LayoutType>;

    using TensorApiBase::TensorApiBase;

    __aicore__ inline LocalTensor() = default;
};

} // namespace AscendC

#endif // IMPL_TENSOR_API_TENSOR_TENSOR_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
