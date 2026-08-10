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
    "impl/tensor_api/atom/copy_traits_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file copy_traits_impl.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ATOM_COPY_TRAITS_IMPL_H
#define IMPL_TENSOR_API_ATOM_COPY_TRAITS_IMPL_H

#include "impl/tensor_api/utils/utils_impl.h"

namespace asc {
namespace te {

template <typename CopyOperation, typename... CopyOpArgs>
struct copy_traits {};

template <typename CopyOp, typename Traits, typename CopyOpWith, typename TraitsWith>
struct copy_traits<CopyOp, Traits, CopyOpWith, TraitsWith> {
    using trait_type = get_trait_member_type_t<Traits>;
    using TraitType = trait_type;
    static constexpr const trait_type default_trait = Traits::value;

    __aicore__ inline constexpr copy_traits<CopyOpWith, TraitsWith> with() const
    {
        return {};
    }

    template <typename Params>
    __aicore__ inline constexpr copy_traits<CopyOpWith, TraitsWith> with(const Params& params) const
    {
        using params_type = Std::remove_cvref_t<Params>;
        if constexpr (Std::is_same_v<params_type, fixpipe_params> || Std::is_same_v<params_type, FixpipeParams>) {
            return {normalize_fixpipe_params(params)};
        } else if constexpr (Std::is_same_v<params_type, copy_gm_to_ub_params>
                             || Std::is_same_v<params_type, CopyGM2UBParams>) {
            return {normalize_copy_gm_to_ub_params(params)};
        } else {
            return {params};
        }
    }

    template <const trait_type& trait = default_trait, typename... Args>
    __aicore__ inline void copy_unpack(const Args&... args) const
    {
        CopyOp::template copy<trait_type, trait, Args...>(args...);
    }
};

} // namespace te
} // namespace asc

#endif // IMPL_TENSOR_API_ATOM_COPY_TRAITS_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
