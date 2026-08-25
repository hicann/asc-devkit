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
    "impl/tensor_api/arch/cube/mmad/mmad.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file mmad.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_MMAD_MMAD_H
#define IMPL_TENSOR_API_ARCH_CUBE_MMAD_MMAD_H

#include "impl/tensor_api/arch/cube/mmad/routing.h"

namespace AscendC {
namespace Te {

constexpr MmadTrait DEFAULT_MMAD_TRAIT;

constexpr MmadParams defaultMmadParams = {0, 0, 0, 0, true};

constexpr MmadParams defaultMmadWithBiasParams = {0, 0, 0, 0, false};

struct MmadTraitDefault {
    using TraitType = MmadTrait;
    static constexpr const TraitType value = DEFAULT_MMAD_TRAIT;
};

struct MmadOperation {
public:
    template <typename Tp, const Tp& traits, typename... Args>
    __aicore__ inline static void Mmad(const Args&... args)
    {
        if ASCEND_IS_AIC {
            MmadImpl<traits, Args...>(args...);
        }
    }

private:
    template <const MmadTrait& trait = DEFAULT_MMAD_TRAIT, typename T, typename U, typename S, typename Params>
    __aicore__ inline static void MmadImpl(const T& dst, const U& fm, const S& filter, const Params& params)
    {
        using dstPos = GetMemLocation<T>;
        using fmPos = GetMemLocation<U>;
        using filterPos = GetMemLocation<S>;
        static_assert(Std::is_same_v<dstPos, Location::L0C>, "When Mmad, dst tensor must be from L0C.");
        static_assert(Std::is_same_v<fmPos, Location::L0A>, "When Mmad, fm tensor must be from L0A.");
        static_assert(Std::is_same_v<filterPos, Location::L0B>, "When Mmad, filter tensor must be from L0B.");
        using DstLayout = typename T::layoutType;
        using FmLayout = typename U::layoutType;
        using FilterLayout = typename S::layoutType;
        using DstLayoutPtn = GetLayoutPattern<DstLayout>;
        using FmLayoutPtn = GetLayoutPattern<FmLayout>;
        using FilterLayoutPtn = GetLayoutPattern<FilterLayout>;
        using MmadImpl = typename MmadRouting<
            CURRENT_ARCH_VERSION, DstLayoutPtn, FmLayoutPtn, FilterLayoutPtn, Location::INVALID>::type;
        MmadImpl::template Run<trait>(dst, fm, filter, params);
    }

    template <
        const MmadTrait& trait = DEFAULT_MMAD_TRAIT, typename T, typename U, typename S, typename V, typename Params>
    __aicore__ inline static void MmadImpl(
        const T& dst, const U& fm, const S& filter, const V& bias, const Params& params)
    {
        using dstPos = GetMemLocation<T>;
        using fmPos = GetMemLocation<U>;
        using filterPos = GetMemLocation<S>;
        using biasPos = GetMemLocation<V>;
        static_assert(Std::is_same_v<dstPos, Location::L0C>, "When Mmad, dst tensor must be from L0C.");
        static_assert(Std::is_same_v<fmPos, Location::L0A>, "When Mmad, fm tensor must be from L0A.");
        static_assert(Std::is_same_v<filterPos, Location::L0B>, "When Mmad, filter tensor must be from L0B.");
        static_assert(
            Std::is_same_v<biasPos, Location::L0C> || Std::is_same_v<biasPos, Location::BIAS>,
            "When Mmad, bias tensor must be from L0C or BIAS.");
        using DstLayout = typename T::layoutType;
        using FmLayout = typename U::layoutType;
        using FilterLayout = typename S::layoutType;
        using DstLayoutPtn = GetLayoutPattern<DstLayout>;
        using FmLayoutPtn = GetLayoutPattern<FmLayout>;
        using FilterLayoutPtn = GetLayoutPattern<FilterLayout>;
        using MmadImpl =
            typename MmadRouting<CURRENT_ARCH_VERSION, DstLayoutPtn, FmLayoutPtn, FilterLayoutPtn, biasPos>::type;
        MmadImpl::template Run<trait>(dst, fm, filter, bias, params);
    }
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_CUBE_MMAD_MMAD_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
