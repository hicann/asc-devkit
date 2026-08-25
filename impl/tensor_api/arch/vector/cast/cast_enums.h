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
    "impl/tensor_api/arch/vector/cast/cast_enums.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file cast_enums.h
 * \brief Cast trait enums — rounding mode, saturation mode, index position, and default trait.
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_CAST_CAST_ENUMS_H
#define IMPL_TENSOR_API_ARCH_VECTOR_CAST_CAST_ENUMS_H

namespace AscendC {
namespace Te {

namespace CastRoundMode {
struct RD {};
struct RN {};
struct RNA {};
struct RH {};
struct RO {};
struct RU {};
struct RZ {};
} // namespace CastRoundMode

namespace CastSatMode {
struct Sat {};
struct NoSat {};
} // namespace CastSatMode

namespace CastIndexPos {
struct Even {};
struct Odd {};
struct PartP0 {};
struct PartP1 {};
struct PartP2 {};
struct PartP3 {};
} // namespace CastIndexPos

struct CastTraitDefault {
    using roundMode = Std::ignore_t;
    using satMode = Std::ignore_t;
    using indexPos = Std::ignore_t;
};

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_VECTOR_CAST_CAST_ENUMS_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
