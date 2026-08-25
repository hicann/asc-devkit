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
    "impl/tensor_api/utils/map_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file map_impl.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_UTILS_MAP_IMPL_H
#define IMPL_TENSOR_API_UTILS_MAP_IMPL_H

#include "impl/tensor_api/utils/extra_impl.h"

namespace AscendC {
namespace Te {

template <typename... Pairs>
class TupleMap {
private:
    using MapType = Std::tuple<Pairs...>;
    static constexpr size_t MapSize = sizeof...(Pairs);

    template <typename Pair>
    using PairKey = typename Std::tuple_element<0, Pair>::type;

    template <typename Pair>
    using PairValue = typename Std::tuple_element<1, Pair>::type;

    template <typename Pair>
    struct IsValidPair : Std::false_type {};

    template <typename Key, typename Value>
    struct IsValidPair<Std::tuple<Key, Value>> : Std::true_type {};

    template <typename Input, typename KeySet>
    struct ContainsKey : Std::false_type {};

    template <typename Input, typename... Keys>
    struct ContainsKey<Input, Std::tuple<Keys...>> : Std::bool_constant<(Std::is_same_v<Input, Keys> || ...)> {};

    template <typename Input, typename KeySet, bool IsTupleKey>
    struct MatchKeyImpl : Std::is_same<KeySet, Input> {};

    template <typename Input, typename KeySet>
    struct MatchKeyImpl<Input, KeySet, true> : ContainsKey<Input, KeySet> {};

    template <typename Input, typename KeySet>
    struct MatchKey : MatchKeyImpl<Input, KeySet, Std::is_tuple_v<KeySet>> {};

    template <typename Pair, typename Map>
    struct PrependPair;

    template <typename Pair, typename... ExistingPairs>
    struct PrependPair<Pair, TupleMap<ExistingPairs...>> {
        using type = TupleMap<Pair, ExistingPairs...>;
    };

    template <typename Key, size_t Index, size_t MaxSize>
    struct GetImpl {
        using CurrentPair = typename Std::tuple_element<Index, MapType>::type;
        static_assert(IsValidPair<CurrentPair>::value, "TupleMap expects Std::tuple<Key, Value> entries.");
        using CurrentKey = PairKey<CurrentPair>;
        using CurrentVal = PairValue<CurrentPair>;

        using NextResult = typename GetImpl<Key, Index + 1, MaxSize>::type;

        using type = Std::conditional_t<Std::is_same_v<CurrentKey, Key>, CurrentVal, NextResult>;
    };

    template <typename Key, size_t MaxSize>
    struct GetImpl<Key, MaxSize, MaxSize> {
        using type = Std::ignore_t;
    };

    template <typename Input, size_t Index, size_t MaxSize>
    struct MatchImpl {
        using CurrentPair = typename Std::tuple_element<Index, MapType>::type;
        static_assert(IsValidPair<CurrentPair>::value, "TupleMap expects Std::tuple<Key, Value> entries.");
        using CurrentKey = PairKey<CurrentPair>;
        using CurrentVal = PairValue<CurrentPair>;

        using NextResult = typename MatchImpl<Input, Index + 1, MaxSize>::type;

        using type = Std::conditional_t<MatchKey<Input, CurrentKey>::value, CurrentVal, NextResult>;
    };

    template <typename Input, size_t MaxSize>
    struct MatchImpl<Input, MaxSize, MaxSize> {
        using type = Std::ignore_t;
    };

    template <typename Key, typename... ExistingPairs>
    struct DeleteImpl;

    template <typename Key, typename Val, typename... ExistingPairs>
    struct InsertImpl;

    template <typename Key, typename Val>
    struct InsertImpl<Key, Val> {
        using type = TupleMap<Std::tuple<Key, Val>>;
    };

    template <typename Key, typename Val, typename FirstPair, typename... RestPairs>
    struct InsertImpl<Key, Val, FirstPair, RestPairs...> {
        static_assert(IsValidPair<FirstPair>::value, "TupleMap expects Std::tuple<Key, Value> entries.");
        using FirstKey = PairKey<FirstPair>;
        using NewPair = Std::tuple<Key, Val>;

        using NextMap = typename InsertImpl<Key, Val, RestPairs...>::type;
        using KeepHeadMap = typename PrependPair<FirstPair, NextMap>::type;
        using ReplaceTailMap = typename DeleteImpl<Key, RestPairs...>::type;
        using ReplaceHeadMap = typename PrependPair<NewPair, ReplaceTailMap>::type;

        using type = Std::conditional_t<Std::is_same_v<FirstKey, Key>, ReplaceHeadMap, KeepHeadMap>;
    };

    template <typename Key>
    struct DeleteImpl<Key> {
        using type = TupleMap<>;
    };

    template <typename Key, typename FirstPair, typename... RestPairs>
    struct DeleteImpl<Key, FirstPair, RestPairs...> {
        static_assert(IsValidPair<FirstPair>::value, "TupleMap expects Std::tuple<Key, Value> entries.");
        using FirstKey = PairKey<FirstPair>;

        using NextMap = typename DeleteImpl<Key, RestPairs...>::type;
        using KeepHeadMap = typename PrependPair<FirstPair, NextMap>::type;

        using type = Std::conditional_t<Std::is_same_v<FirstKey, Key>, NextMap, KeepHeadMap>;
    };

public:
    template <typename Key>
    using Get = typename GetImpl<Key, 0, MapSize>::type;

    template <typename Input>
    using Find = typename MatchImpl<Input, 0, MapSize>::type;

    template <typename Key, typename Val>
    using Insert = typename InsertImpl<Key, Val, Pairs...>::type;

    template <typename Key>
    using Delete = typename DeleteImpl<Key, Pairs...>::type;
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_UTILS_MAP_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
