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
    "impl/tensor_api/atom/cube/copy_l0c2l1.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file copy_l0c2l1.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ATOM_CUBE_COPY_L0C2L1_H
#define IMPL_TENSOR_API_ATOM_CUBE_COPY_L0C2L1_H

#include "impl/tensor_api/utils/utils_impl.h"
#include "impl/tensor_api/arch/cube/l0c_to_l1/copy.h"
#include "impl/tensor_api/atom/copy_traits_impl.h"

namespace AscendC {
namespace Te {

template <typename TraitStruct>
struct CopyTraits<CopyL0C2L1With, TraitStruct> {
    using TraitType = typename TraitStruct::TraitType;
    static constexpr const TraitType defaultTrait = TraitStruct::value;

    template <const TraitType& trait = defaultTrait, typename... Args>
    __aicore__ inline void CopyUnpack(const Args&... args) const
    {
        CopyL0C2L1With::Copy<TraitType, trait, Args...>(args..., params);
    }
    FixpipeParams params;
};

template <typename Traits>
struct CopyTraits<CopyL0C2L1, Traits> : public CopyTraits<CopyL0C2L1, Traits, CopyL0C2L1With, Traits> {};

template <>
struct CopyTraits<CopyL0C2L1> : public CopyTraits<CopyL0C2L1, CopyL0C2L1TraitDefault> {};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ATOM_CUBE_COPY_L0C2L1_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
