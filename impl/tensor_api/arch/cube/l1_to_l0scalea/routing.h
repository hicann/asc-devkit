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
    "impl/tensor_api/arch/cube/l1_to_l0scalea/routing.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file routing.h
 * \brief
 */

#ifndef IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0SCALEA_ROUTING_H
#define IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0SCALEA_ROUTING_H

#include "impl/tensor_api/arch/cube/l1_to_l0scalea/copy_impl/scalea.h"

namespace AscendC {
namespace Te {

class CopyL12L0ScaleAIgnore {
public:
    template <const CopyL12L0ScaleATrait& trait, typename... Args>
    __aicore__ inline void static Run(const Args&... args)
    {
        static_assert(Std::is_same_v<Args..., void>, "CopyL12L0ScaleAIgnore should not be called");
    }
};

template <uint32_t Version, typename DstLayoutPattern, typename SrcLayoutPattern>
struct CopyL12L0ScaleARouting {
    using type = CopyL12L0ScaleAIgnore;
};

template <uint32_t Version>
struct CopyL12L0ScaleARouting<Version, ZZLayoutPtn, ZZLayoutPtn> {
    using type = LoadDataL12L0MxScaleA3510;
};

} // namespace Te
} // namespace AscendC
#endif // IMPL_TENSOR_API_ARCH_CUBE_L1_TO_L0SCALEA_ROUTING_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
