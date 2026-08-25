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
    "impl/tensor_api/atom/copy_atom_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file copy_atom_impl.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ATOM_COPY_ATOM_IMPL_H
#define IMPL_TENSOR_API_ATOM_COPY_ATOM_IMPL_H

#include "impl/tensor_api/atom/cube/copy_gm2l1.h"

#include "impl/tensor_api/atom/cube/copy_l0c2gm.h"
#include "impl/tensor_api/atom/cube/copy_l0c2ub.h"
#include "impl/tensor_api/atom/cube/copy_l0c2l1.h"
#include "impl/tensor_api/atom/cube/copy_l12ub.h"

#include "impl/tensor_api/atom/vector/copy_gm2ub.h"
#include "impl/tensor_api/atom/vector/copy_ub2gm.h"
#include "impl/tensor_api/atom/vector/copy_ub2l1.h"
#include "impl/tensor_api/atom/vector/copy_ub2ub.h"

#include "impl/tensor_api/atom/cube/copy_l12bt.h"
#include "impl/tensor_api/atom/cube/copy_l12fb.h"

#include "impl/tensor_api/atom/cube/copy_l12l0a.h"
#include "impl/tensor_api/atom/cube/copy_l12l0b.h"
#include "impl/tensor_api/atom/cube/copy_l12l0scalea.h"
#include "impl/tensor_api/atom/cube/copy_l12l0scaleb.h"

namespace AscendC {
namespace Te {

template <typename... Args>
struct CopyAtom;

template <typename CopyOperation>
struct CopyAtom<CopyOperation> : CopyAtom<CopyTraits<CopyOperation>> {};

template <typename... Args>
struct CopyAtom<CopyTraits<Args...>> : CopyTraits<Args...> {
    using CopyTraitType = CopyTraits<Args...>;
    using TraitType = typename CopyTraitType::TraitType;
    static constexpr const TraitType defaultTrait = CopyTraitType::defaultTrait;

    template <const TraitType& traits = defaultTrait, typename... Params>
    __aicore__ inline void Call(const Params&... params) const
    {
        CopyTraitType::template CopyUnpack<traits>(params...);
    }

    template <typename... TraitsArgs>
    __aicore__ inline auto with(TraitsArgs&&... args) const
    {
        auto traits = CopyTraitType::with(static_cast<TraitsArgs&&>(args)...);
        return CopyAtom<decltype(traits)>{traits};
    }
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ATOM_COPY_ATOM_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
