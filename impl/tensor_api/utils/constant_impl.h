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
    "impl/tensor_api/utils/constant_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file constant_impl.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_UTILS_CONSTANT_IMPL_H
#define IMPL_TENSOR_API_UTILS_CONSTANT_IMPL_H

#include "impl/tensor_api/utils/extra_impl.h"
#include "impl/tensor_api/utils/map_impl.h"

namespace asc {
namespace te {
namespace inst {}

using Std::Int;
using _0 = Std::Int<0>;
using _1 = Std::Int<1>;
using _2 = Std::Int<2>;
using _3 = Std::Int<3>;
using _4 = Std::Int<4>;
using _5 = Std::Int<5>;
using _6 = Std::Int<6>;
using _7 = Std::Int<7>;
using _8 = Std::Int<8>;
using _9 = Std::Int<9>;
using _10 = Std::Int<10>;
using _16 = Std::Int<16>;
using _24 = Std::Int<24>;
using _32 = Std::Int<32>;
using _64 = Std::Int<64>;
using _128 = Std::Int<128>;
using _256 = Std::Int<256>;
using _512 = Std::Int<512>;
using _1024 = Std::Int<1024>;
using _2048 = Std::Int<2048>;
using _4096 = Std::Int<4096>;

// Squeeze pattern mark: positions marked with `_` are kept (only `_1` marks a squeeze).
struct keep_mark {
    __aicore__ inline constexpr keep_mark() = default;
};
inline constexpr keep_mark _{};

constexpr size_t TWO_DIM_DATA = 2;
constexpr size_t THREE_DIM_DATA = 3;
constexpr size_t FOUR_DIM_DATA = 4;
constexpr size_t FIVE_DIM_DATA = 5;
constexpr size_t FRACTAL_FIXED = 16;
constexpr size_t MX_SCALE_K0 = 2;
constexpr uint32_t BLOCK_CUBE = 16;
constexpr uint64_t HIFLOAT8_MMAD_CTRL_MASK = 0x200000000000ULL;

struct arch_version {
    static constexpr uint32_t V3510 = 3510;
    static constexpr uint32_t V2201 = 2201;
};

struct get_arch_version {
    __aicore__ inline constexpr uint32_t operator()() const
    {
#ifdef __NPU_ARCH__
        return __NPU_ARCH__;
#else
        return 0;
#endif
    }
};

constexpr uint32_t CURRENT_ARCH_VERSION = get_arch_version{}();

namespace copy_mode {
struct normal {};
struct trans {};
struct trans_b8b4 {};
}; // namespace copy_mode

namespace location {
struct invalid {};
struct gm {};
struct ub {};
struct l1 {};
struct l0a {};
struct l0b {};
struct l0scalea {};
struct l0scaleb {};
struct l0c {};
struct bias {};
struct fixbuf {};
struct ssbuf {};
} // namespace location

template <typename T>
struct is_hardware {
private:
    template <typename Tp, typename... Tps>
    __aicore__ inline static constexpr bool is_unqualified_any_of()
    {
        return (... || Std::is_same_v<Std::remove_cvref_t<Tp>, Tps>);
    }

public:
    static constexpr bool value =
        is_unqualified_any_of<T, location::invalid, location::gm, location::ub, location::l1, location::l0a,
                              location::l0b, location::l0scalea, location::l0scaleb, location::l0c, location::bias,
                              location::fixbuf, location::ssbuf>();
};

template <typename T>
constexpr bool is_hardware_v = is_hardware<T>::value;

template <typename TupleType>
using tuple_sequence = Std::make_index_sequence<Std::tuple_size_v<Std::remove_cvref_t<TupleType>>>;

template <typename T>
struct location_attr {
    using gm_attr = __gm__ T*;
    using l1_attr = __cbuf__ T*;
    using l0a_attr = __ca__ T*;
    using l0b_attr = __cb__ T*;
    using l0c_attr = __cc__ T*;
    using ub_attr = __ubuf__ T*;
    using fixbuf_attr = __fbuf__ T*;
    using ssbuf_attr = __ssbuf__ T*;
    using bias_attr = __biasbuf__ T*;
    using none_attr = T*;

    using type = Std::tuple<gm_attr, l1_attr, l0a_attr, l0b_attr, l0c_attr, ub_attr, fixbuf_attr, ssbuf_attr, bias_attr>;

    using location_map = tuple_map<Std::tuple<location::gm, gm_attr>, Std::tuple<location::l1, l1_attr>,
                                  Std::tuple<location::l0a, l0a_attr>, Std::tuple<location::l0b, l0b_attr>,
                                  Std::tuple<location::l0scalea, none_attr>, Std::tuple<location::l0scaleb, none_attr>,
                                  Std::tuple<location::l0c, l0c_attr>, Std::tuple<location::ub, ub_attr>,
                                  Std::tuple<location::bias, bias_attr>, Std::tuple<location::fixbuf, fixbuf_attr>>;
};

template <typename A, typename ProcessedTuple>
struct all_elements_same_as_a;

template <typename A, typename First, typename... Rest>
struct all_elements_same_as_a<A, Std::tuple<First, Rest...>> {
    static constexpr bool value =
        Std::is_same_v<A, typename iter_ele<First>::type> || all_elements_same_as_a<A, Std::tuple<Rest...>>::value;
};

template <typename A>
struct all_elements_same_as_a<A, Std::tuple<>> {
    static constexpr bool value = false;
};

template <typename A, typename... BList>
struct check_all_same;

template <typename A, typename B, typename... RestB>
struct check_all_same<A, B, RestB...> {
    static constexpr bool value = Std::is_same_v<A, B>
                                  || all_elements_same_as_a<A, typename location_attr<B>::type>::value
                                  || check_all_same<A, RestB...>::value;
};

template <typename A>
struct check_all_same<A> {
    static constexpr bool value = false;
};

template <typename A, typename... BList>
constexpr bool is_one_of_attr_v = check_all_same<A, BList...>::value;

template <typename DataType>
inline constexpr bool is_data_type =
    is_one_of_attr_v<Std::remove_cvref_t<DataType>, hifloat8_t, bfloat16_t, fp4x2_e1m2_t, fp4x2_e2m1_t, fp8_e5m2_t,
                     fp8_e4m3fn_t, fp8_e8m0_t>
    || Std::is_integral_v<Std::remove_cvref_t<DataType>> || Std::is_floating_point_v<Std::remove_cvref_t<DataType>>;

template <typename DataType>
inline constexpr bool is_b4_type = is_one_of_attr_v<DataType, fp4x2_e1m2_t, fp4x2_e2m1_t>;

template <typename T = Std::ignore_t>
__aicore__ inline constexpr size_t get_c0_size()
{
    constexpr size_t c0_size = 32;
    if constexpr (is_b4_type<T>) {
        return c0_size * 2;
    } else {
        return c0_size;
    }
}

template <typename T = Std::ignore_t>
constexpr size_t C0_SIZE = get_c0_size<T>();

template <typename T>
constexpr size_t C0_ELEMENT = C0_SIZE<T> / sizeof(T);

// is_integral_constant
template <typename T>
struct is_integral_constant : Std::false_type {};

template <size_t Value>
struct is_integral_constant<Std::Int<Value>> : Std::true_type {};

template <typename T>
constexpr bool is_integral_constant_v = is_integral_constant<T>::value;

} // namespace te
} // namespace asc



#endif // IMPL_TENSOR_API_UTILS_CONSTANT_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
