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
    "impl/tensor_api/arch/cube/mmad/routing.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file routing.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_CUBE_MMAD_ROUTING_H
#define IMPL_TENSOR_API_ARCH_CUBE_MMAD_ROUTING_H

#include "impl/tensor_api/arch/cube/mmad/mmad_impl/mmad.h"
#include "impl/tensor_api/arch/cube/mmad/mmad_impl/mmad_with_bias.h"

namespace AscendC {
namespace Te {

class MmadIgnore {
public:
    template <const MmadTrait& trait, typename... Args>
    __aicore__ inline static void Run(const Args&... args)
    {
        static_assert(Std::is_same_v<Args..., void>, "MmadIgnore should not be called");
    }
};

template <uint32_t Version, typename DstLayoutPtn, typename FmLayoutPtn, typename FilterLayoutPtn, typename BiasPos>
struct MmadRouting {
    using type = MmadIgnore;
};

template <uint32_t Version>
struct MmadRouting<Version, NZLayoutPtn, NZLayoutPtn, ZNLayoutPtn, Location::INVALID> {
    using type = Mmad;
};

template <uint32_t Version>
struct MmadRouting<Version, NZLayoutPtn, NZLayoutPtn, ZNLayoutPtn, Location::L0C> {
    using type = MmadWithBias;
};

template <uint32_t Version>
struct MmadRouting<Version, NZLayoutPtn, NZLayoutPtn, ZNLayoutPtn, Location::BIAS> {
    using type = MmadWithBias;
};
} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_CUBE_MMAD_ROUTING_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
