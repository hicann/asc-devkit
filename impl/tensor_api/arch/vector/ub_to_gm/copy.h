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
    "impl/tensor_api/arch/vector/ub_to_gm/copy.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file copy.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_UB_TO_GM_COPY_H
#define IMPL_TENSOR_API_ARCH_VECTOR_UB_TO_GM_COPY_H

#include "impl/tensor_api/atom/copy_traits_impl.h"
#include "impl/tensor_api/arch/vector/ub_to_gm/routing.h"

namespace AscendC {
namespace Te {

constexpr CopyUB2GMTrait DEFAULT_COPY_UB_TO_GM_TRAIT;

struct CopyUB2GMTraitDefault {
    using TraitType = CopyUB2GMTrait;
    static constexpr const TraitType value = DEFAULT_COPY_UB_TO_GM_TRAIT;
};

struct CopyUB2GM {
public:
    template <typename Tp, const Tp& traits, typename... Args>
    __aicore__ inline static void Copy(const Args&... args)
    {
        if ASCEND_IS_AIV {
            DataCopyImpl<traits, Args...>(args...);
        }
    }

private:
    template <const CopyUB2GMTrait& trait = DEFAULT_COPY_UB_TO_GM_TRAIT, typename T, typename U>
    __aicore__ inline static void DataCopyImpl(const T& dst, const U& src)
    {
        using dstTPos = GetMemLocation<T>;
        using srcTPos = GetMemLocation<U>;
        static_assert(Std::is_same_v<dstTPos, Location::GM>, "When Copy tensor from UB to GM, dst tensor must on GM");
        static_assert(Std::is_same_v<srcTPos, Location::UB>, "When Copy tensor from UB to GM, src tensor must on UB");
        using DstLayoutPtn = GetLayoutPattern<typename T::layoutType>;
        using SrcLayoutPtn = GetLayoutPattern<typename U::layoutType>;
        using CopyUB2GMImpl = typename CopyUB2GMRouting<CURRENT_ARCH_VERSION, DstLayoutPtn, SrcLayoutPtn>::type;
        CopyUB2GMImpl::template Run<trait, T, U>(dst, src);
    }
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_VECTOR_UB_TO_GM_COPY_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
