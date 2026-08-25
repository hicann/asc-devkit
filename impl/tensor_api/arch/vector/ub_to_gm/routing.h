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
    "impl/tensor_api/arch/vector/ub_to_gm/routing.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file routing.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_UB_TO_GM_ROUTING_H
#define IMPL_TENSOR_API_ARCH_VECTOR_UB_TO_GM_ROUTING_H

#include "impl/tensor_api/arch/vector/ub_to_gm/copy_impl/dn2dn.h"
#include "impl/tensor_api/arch/vector/ub_to_gm/copy_impl/nd2nd.h"
#include "impl/tensor_api/arch/vector/ub_to_gm/copy_impl/nz2nz.h"

namespace AscendC {
namespace Te {

class CopyUB2GMIgnore {
public:
    template <const CopyUB2GMTrait& trait, typename... Args>
    __aicore__ inline static void Run(const Args&... args)
    {
        static_assert(Std::is_same_v<Args..., void>, "CopyUB2GMIgnore should not be called");
    }
};

template <uint32_t Version, typename DstLayoutPtn, typename SrcLayoutPtn>
struct CopyUB2GMRouting {
    using type = CopyUB2GMIgnore;
};

template <uint32_t Version>
struct CopyUB2GMRouting<Version, NDExtLayoutPtn, NDExtLayoutPtn> {
    using type = CopyUbufToGmAlignV2ND;
};

template <uint32_t Version>
struct CopyUB2GMRouting<Version, DNExtLayoutPtn, DNExtLayoutPtn> {
    using type = CopyUbufToGmAlignV2DN;
};

template <uint32_t Version>
struct CopyUB2GMRouting<Version, NZLayoutPtn, NZLayoutPtn> {
    using type = CopyUbufToGmAlignV2NZ;
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_VECTOR_UB_TO_GM_ROUTING_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
