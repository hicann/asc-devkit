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
#include "impl/tensor_api/utils/npu_debug_assert.h"
#include "impl/tensor_api/utils/npu_debug_check.h"
#include "impl/tensor_api/tensor/layout_impl.h"
#include "impl/tensor_api/tensor/engine_impl.h"
#include "impl/tensor_api/tensor/pointer_pattern.h"
#include "impl/tensor_api/experimental/reg/load_mode.h"
#include "impl/tensor_api/experimental/reg/mask_impl.h"

namespace asc {
namespace te {

template <typename EngineType, typename LayoutType>
struct global_tensor;

template <typename EngineType, typename LayoutType>
struct local_tensor;

template <typename LocationType, typename EngineType, typename LayoutType>
struct make_tensor_result;

template <typename EngineType, typename LayoutType>
struct base_tensor {
    using iterator = typename EngineType::iterator;
    using value_type = typename EngineType::value_type;
    using element_type = typename EngineType::element_type;
    using elementType = element_type;
    using reference = typename EngineType::reference;

    using engine_type = EngineType;
    using layout_type = LayoutType;

    __aicore__ inline base_tensor() {}
    __aicore__ inline base_tensor(const EngineType& engine, const LayoutType& layout) : rep(layout, engine) {}

    static constexpr int rank = LayoutType::rank; // tuple size

    __aicore__ inline constexpr decltype(auto) tensor() const
    {
        return *this;
    }

    __aicore__ inline constexpr decltype(auto) engine() const
    {
        return Std::get<1>(rep);
    }

    __aicore__ inline constexpr decltype(auto) engine()
    {
        return Std::get<1>(rep);
    }

    __aicore__ inline constexpr decltype(auto) layout() const
    {
        return Std::get<0>(rep);
    }

    __aicore__ inline constexpr decltype(auto) data() const
    {
        return engine().begin();
    }

    __aicore__ inline constexpr decltype(auto) data()
    {
        return engine().begin();
    }

    __aicore__ inline constexpr decltype(auto) shape() const
    {
        return layout().shape();
    }

    __aicore__ inline constexpr decltype(auto) stride() const
    {
        return layout().stride();
    }

    __aicore__ inline constexpr auto size() const
    {
        return layout().size();
    }

    __aicore__ inline constexpr auto capacity() const
    {
        return layout().capacity();
    }

    template <typename Coord>
    __aicore__ inline constexpr decltype(auto) operator[](const Coord& coord)
    {
        TENSOR_API_DEBUG_CHECK(debug_check_coord, layout(), coord, "tensor operator[]");
        return data()[layout()(coord)];
    }

    template <typename Coord>
    __aicore__ inline constexpr decltype(auto) operator[](const Coord& coord) const
    {
        TENSOR_API_DEBUG_CHECK(debug_check_coord, layout(), coord, "tensor operator[]");
        return data()[layout()(coord)];
    }

    using data_type = get_attribute_element_type<element_type*>;

    template <load_sideband_mode sideband_mode = load_sideband_mode::direct, typename Coord>
    __simd_callee__ inline decltype(auto) load(const Coord& coord) const
    {
        static_assert(Std::is_same_v<get_mem_location<EngineType>, location::ub>,
                      "base_tensor::load only supports tensors located in UB");

        TENSOR_API_DEBUG_CHECK(debug_check_coord, layout(), coord, "tensor::load");
        reg_tensor<data_type> dst;
        auto src_engine = engine() + layout()(coord);
        auto src = src_engine.begin().get();
        if constexpr (sideband_mode == load_sideband_mode::direct) {
            asc_loadalign(dst.reg, src);
        }
        return dst;
    }

    template <typename Coord, typename RegDataType>
    __simd_callee__ inline void store(const Coord& coord, const reg_tensor<RegDataType>& src)
    {
        static_assert(Std::is_same_v<get_mem_location<EngineType>, location::ub>,
                      "base_tensor::store only supports tensors located in UB");
        static_assert(Std::is_same_v<data_type, RegDataType>,
                      "base_tensor::store requires the tensor and reg_tensor to have the same element type");

        TENSOR_API_DEBUG_CHECK(debug_check_coord, layout(), coord, "tensor::store");
        auto dst_engine = engine() + layout()(coord);
        asc_storealign(dst_engine.begin().get(), src.reg, src.mask);
    }

    template <typename Coord>
    __aicore__ inline constexpr decltype(auto) operator()(const Coord& coord)
    {
        TENSOR_API_DEBUG_CHECK(debug_check_coord, layout(), coord, "tensor operator()");
        auto slice_engine = engine() + layout()(coord);
        auto coord_layout = make_coord_layout(coord, layout());
        return make_sub_tensor(slice_engine, coord_layout);
    }

    template <typename Coord>
    __aicore__ inline constexpr decltype(auto) operator()(const Coord& coord) const
    {
        TENSOR_API_DEBUG_CHECK(debug_check_coord, layout(), coord, "tensor operator()");
        auto slice_engine = engine() + layout()(coord);
        auto coord_layout = make_coord_layout(coord, layout());
        return make_sub_tensor(slice_engine, coord_layout);
    }

    template <typename Coord0, typename Coord1, typename... Coords>
    __aicore__ inline constexpr decltype(auto) operator()(const Coord0& c0, const Coord1& c1, const Coords&... cs)
    {
        return operator()(make_coord(c0, c1, cs...));
    }

    template <typename Coord0, typename Coord1, typename... Coords>
    __aicore__ inline constexpr decltype(auto) operator()(const Coord0& c0, const Coord1& c1, const Coords&... cs) const
    {
        return operator()(make_coord(c0, c1, cs...));
    }

    template <typename Coord, typename Info>
    __aicore__ inline constexpr decltype(auto) slice(const Coord& coord, const Info& info)
    {
        TENSOR_API_DEBUG_CHECK(debug_check_slice_args, layout(), coord, info);
        auto slice_engine = engine() + layout()(coord);
        auto coord_layout = make_slice_layout(coord, layout(), info);
        return make_sub_tensor(slice_engine, coord_layout);
    }

    template <typename Coord, typename Info>
    __aicore__ inline constexpr decltype(auto) slice(const Coord& coord, const Info& info) const
    {
        TENSOR_API_DEBUG_CHECK(debug_check_slice_args, layout(), coord, info);
        auto slice_engine = engine() + layout()(coord);
        auto coord_layout = make_slice_layout(coord, layout(), info);
        return make_sub_tensor(slice_engine, coord_layout);
    }

    __aicore__ inline constexpr decltype(auto) Tensor() const
    {
        return tensor();
    }

    __aicore__ inline constexpr decltype(auto) Engine() const
    {
        return engine();
    }

    __aicore__ inline constexpr decltype(auto) Engine()
    {
        return engine();
    }

    __aicore__ inline constexpr decltype(auto) Layout() const
    {
        return layout();
    }

    __aicore__ inline constexpr decltype(auto) Data() const
    {
        return data();
    }

    __aicore__ inline constexpr decltype(auto) Data()
    {
        return data();
    }

    __aicore__ inline constexpr decltype(auto) Shape() const
    {
        return shape();
    }

    __aicore__ inline constexpr decltype(auto) Stride() const
    {
        return stride();
    }

    __aicore__ inline constexpr auto Size() const
    {
        return size();
    }

    __aicore__ inline constexpr auto Capacity() const
    {
        return capacity();
    }

    template <typename Coord, typename Info>
    __aicore__ inline constexpr decltype(auto) Slice(const Coord& coord, const Info& info)
    {
        return slice(coord, info);
    }

    template <typename Coord, typename Info>
    __aicore__ inline constexpr decltype(auto) Slice(const Coord& coord, const Info& info) const
    {
        return slice(coord, info);
    }

private:
    template <typename SliceEngine, typename SliceLayout>
    __aicore__ inline static constexpr decltype(auto) make_sub_tensor(const SliceEngine& slice_engine,
                                                                      const SliceLayout& slice_layout)
    {
        using location = get_mem_location<SliceEngine>;
        using result_tensor = typename make_tensor_result<location, SliceEngine, SliceLayout>::type;
        return result_tensor{slice_engine, slice_layout};
    }

    Std::tuple<layout_type, engine_type> rep;
};

template <typename EngineType, typename LayoutType>
struct global_tensor : public base_tensor<EngineType, LayoutType> {
    using tensor_api_base = base_tensor<EngineType, LayoutType>;
    using layoutType = typename tensor_api_base::layout_type;

    using tensor_api_base::tensor_api_base;

    __aicore__ inline global_tensor() = default;

    __aicore__ inline constexpr void set_l2_cache_hint(cache_mode mode)
    {
        this->engine().set_cache_mode(mode);
    }

    __aicore__ inline constexpr void SetL2CacheHint(CacheMode mode)
    {
        set_l2_cache_hint(normalize_cache_mode(mode));
    }
};

template <typename EngineType, typename LayoutType>
struct local_tensor : public base_tensor<EngineType, LayoutType> {
    using tensor_api_base = base_tensor<EngineType, LayoutType>;
    using layoutType = typename tensor_api_base::layout_type;

    using tensor_api_base::tensor_api_base;

    __aicore__ inline local_tensor() = default;
};

template <typename LocationType, typename EngineType, typename LayoutType>
struct make_tensor_result {
    using type =
        typename Std::conditional<Std::is_same_v<LocationType, location::gm>, global_tensor<EngineType, LayoutType>,
                                  local_tensor<EngineType, LayoutType>>::type;
};

template <typename T>
struct is_attr_tensor : Std::false_type {};

template <typename EngineType, typename LayoutType>
struct is_attr_tensor<global_tensor<EngineType, LayoutType>> : Std::true_type {};

template <typename EngineType, typename LayoutType>
struct is_attr_tensor<local_tensor<EngineType, LayoutType>> : Std::true_type {};

template <typename T>
constexpr bool is_attr_tensor_v = is_attr_tensor<Std::remove_cvref_t<T>>::value;

template <typename T>
struct make_tensor_builder {
    template <typename Arg0, typename... Args>
    __aicore__ inline constexpr auto operator()(const Arg0& arg0, const Args&... args) const
    {
        using engine = view_engine<Arg0>;
        if constexpr (sizeof...(Args) == 1 && (is_layout_v<Args> && ...)) {
            using layout = typename Std::tuple_element<0, Std::tuple<Args...>>::type;
            using location = get_mem_location<engine>;
            using result_tensor = typename make_tensor_result<location, engine, layout>::type;
            return result_tensor{engine{arg0}, args...};
        } else if constexpr (sizeof...(Args) == 2 && (Std::is_tuple_v<Std::remove_cvref_t<Args>> && ...)) {
            using layout = decltype(make_layout(args...));
            using location = get_mem_location<engine>;
            using result_tensor = typename make_tensor_result<location, engine, layout>::type;
            return result_tensor{engine{arg0}, make_layout(args...)};
        } else {
            static_assert(
                sizeof...(Args) != sizeof...(Args),
                "make_tensor expects a hardware memory pointer and data structure like Layout or Shape and Stride");
        }
    }
};

template <typename Iterator, typename... Args>
__aicore__ inline constexpr auto make_tensor(const Iterator& iter, const Args&... args)
{
    static_assert(is_hardware_mem_ptr_v<Iterator>,
                  "make_tensor expects the first argument to be a memory pointer or iterator");
    TENSOR_API_DEBUG_CHECK(debug_check_make_tensor_shape, args...);
    return make_tensor_builder<Iterator>{}(iter, args...);
}

// Construct a single-batch sub-tensor from a batched tensor by offsetting the engine pointer
// and stripping the leading batch axis from the layout. Layout depth 5 -> 4 (e.g. NZ fractal)
// or 3 -> 2 (e.g. ND row/col). Other depths trigger a compile-time error.
template <typename Tensor>
__aicore__ inline constexpr auto make_single_batch_sub_tensor(const Tensor& t, uint32_t batch_idx)
{
    using layout_type = typename Tensor::layout_type;
    static_assert(layout_type::depth == THREE_DIM_DATA || layout_type::depth == FIVE_DIM_DATA,
                  "make_single_batch_sub_tensor only supports batched layouts of depth 3 or 5.");
    auto layout = t.layout();
    auto batch_stride = get<0>(layout.stride());
    auto sub_engine = t.engine() + batch_stride * batch_idx;
    auto sub_layout = remove_batch_dim(layout);
    using new_engine = Std::remove_cvref_t<decltype(sub_engine)>;
    using new_layout = Std::remove_cvref_t<decltype(sub_layout)>;
    using location = get_mem_location<new_engine>;
    using new_tensor = typename make_tensor_result<location, new_engine, new_layout>::type;
    return new_tensor{sub_engine, sub_layout};
}

// squeeze (mode 1): drop axes (by index) whose compile-time size is 1; non-one axes are kept (no error).
// Accepts either a Layout (returns a new Layout) or a Tensor (returns a new Tensor over the same engine).
template <size_t... squeeze_dims, typename T,
          typename = Std::enable_if_t<(is_layout_v<T> || is_attr_tensor_v<T>) && (sizeof...(squeeze_dims) > 0)>>
__aicore__ inline constexpr auto squeeze(const T& x)
{
    if constexpr (is_attr_tensor_v<T>) {
        auto new_layout = squeeze_layout<squeeze_dims...>(x.layout());
        using new_layout_type = Std::remove_cvref_t<decltype(new_layout)>;
        using new_engine = typename T::engine_type;
        using location = get_mem_location<new_engine>;
        using new_tensor = typename make_tensor_result<location, new_engine, new_layout_type>::type;
        return new_tensor{x.engine(), new_layout};
    } else {
        static_assert(is_layout_v<T>, "squeeze input must be a Layout or Tensor");
        return squeeze_layout<squeeze_dims...>(x);
    }
}

// squeeze (mode 2): drop positions marked _1 in an isomorphic pattern tuple, when size is 1.
// Accepts either a Layout or a Tensor.
template <typename Pattern, typename T,
          typename = Std::enable_if_t<(is_layout_v<T> || is_attr_tensor_v<T>)
                                      && Std::is_tuple_v<Std::remove_cvref_t<Pattern>>>>
__aicore__ inline constexpr auto squeeze(const T& x, const Pattern& pattern)
{
    if constexpr (is_attr_tensor_v<T>) {
        auto new_layout = squeeze_layout(x.layout(), pattern);
        using new_layout_type = Std::remove_cvref_t<decltype(new_layout)>;
        using new_engine = typename T::engine_type;
        using location = get_mem_location<new_engine>;
        using new_tensor = typename make_tensor_result<location, new_engine, new_layout_type>::type;
        return new_tensor{x.engine(), new_layout};
    } else {
        static_assert(is_layout_v<T>, "squeeze input must be a Layout or Tensor");
        return squeeze_layout(x, pattern);
    }
}

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_TENSOR_TENSOR_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
