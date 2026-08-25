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
    "impl/tensor_api/arch/cube/l1_to_l0scalea/copy.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file copy.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0SCALEA_COPY_H
#define IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0SCALEA_COPY_H

#include "impl/tensor_api/arch/cube/l1_to_l0scalea/routing.h"

namespace AscendC {
namespace Te {

constexpr CopyL12L0ScaleATrait DEFAULT_COPY_L1_TO_L0SCALEA_TRAIT;

struct CopyL12L0ScaleATraitDefault {
    using TraitType = CopyL12L0ScaleATrait;
    static constexpr const TraitType value = DEFAULT_COPY_L1_TO_L0SCALEA_TRAIT;
};

struct CopyL12L0ScaleA {
public:
    template <typename Tp, const Tp& traits, typename... Args>
    __aicore__ inline static void Copy(const Args&... args)
    {
        if ASCEND_IS_AIC {
            LoadData<traits, Args...>(args...);
        }
    }

private:
    template <const CopyL12L0ScaleATrait& trait = DEFAULT_COPY_L1_TO_L0SCALEA_TRAIT, typename T, typename U>
    __aicore__ inline static void LoadData(const T& dst, const U& src)
    {
        using dstPos = GetMemLocation<T>;
        using srcPos = GetMemLocation<U>;
        static_assert(
            Std::is_same_v<dstPos, Location::L0ScaleA>,
            "When Copy tensor from L1 to L0ScaleA, dst tensor must on L0ScaleA");
        static_assert(
            Std::is_same_v<srcPos, Location::L1>, "When Copy tensor from L1 to L0ScaleA, src tensor must on L1");
        using DstLayout = typename T::layoutType;
        using SrcLayout = typename U::layoutType;
        using DstPattern = GetLayoutPattern<DstLayout>;
        using SrcPattern = GetLayoutPattern<SrcLayout>;
        using CopyL12L0ScaleAImpl = typename CopyL12L0ScaleARouting<CURRENT_ARCH_VERSION, DstPattern, SrcPattern>::type;
        CopyL12L0ScaleAImpl::template Run<trait, T, U>(dst, src);
    }
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0SCALEA_COPY_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
