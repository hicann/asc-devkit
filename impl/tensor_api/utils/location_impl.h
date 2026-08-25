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
    "impl/tensor_api/utils/location_impl.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file location_impl.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_UTILS_LOCATION_IMPL_H
#define IMPL_TENSOR_API_UTILS_LOCATION_IMPL_H

#include "impl/tensor_api/utils/map_impl.h"

namespace AscendC {
namespace Te {

#define TENSOR_API_QUAL_LOC_PAIRS(Q) \
    Q(_gm__, GM)                     \
    Q(_ubuf__, UB)                   \
    Q(_cbuf__, L1)                   \
    Q(_ca__, L0A)                    \
    Q(_cb__, L0B)                    \
    Q(_cc__, L0C)                    \
    Q(_biasbuf__, BIAS)              \
    Q(_fbuf__, FIXBUF)               \
    Q(_ssbuf__, SSBUF)

#define TENSOR_API_USER_TYPES(M, ...) \
    M(int8_t, __VA_ARGS__)            \
    M(int16_t, __VA_ARGS__)           \
    M(int32_t, __VA_ARGS__)           \
    M(int64_t, __VA_ARGS__)           \
    M(uint8_t, __VA_ARGS__)           \
    M(uint16_t, __VA_ARGS__)          \
    M(uint32_t, __VA_ARGS__)          \
    M(uint64_t, __VA_ARGS__)          \
    M(half, __VA_ARGS__)              \
    M(float, __VA_ARGS__)             \
    M(hifloat8_t, __VA_ARGS__)        \
    M(bfloat16_t, __VA_ARGS__)        \
    M(fp4x2_e1m2_t, __VA_ARGS__)      \
    M(fp4x2_e2m1_t, __VA_ARGS__)      \
    M(fp8_e5m2_t, __VA_ARGS__)        \
    M(fp8_e4m3fn_t, __VA_ARGS__)      \
    M(fp8_e8m0_t, __VA_ARGS__)

#define TENSOR_API_TYPE_ALIAS(qual, type) qual##_##type

#define TENSOR_API_MAKE_COMBO(qual, type) _##qual type*

#define TENSOR_API_GEN_ONE_TYPE(type, qualifier) \
    using TENSOR_API_TYPE_ALIAS(qualifier, type) = TENSOR_API_MAKE_COMBO(qualifier, type);

#define TENSOR_API_GENERATE_ALL_TYPES(qual, loc) TENSOR_API_USER_TYPES(TENSOR_API_GEN_ONE_TYPE, qual)

#define TENSOR_API_GEN_QUAL_TYPE_TUPLE(type, qual, loc) Std::tuple<TENSOR_API_TYPE_ALIAS(qual, type), Location::loc>,

#define TENSOR_API_PAIR_QUAL_LOC(qual, loc) TENSOR_API_USER_TYPES(TENSOR_API_GEN_QUAL_TYPE_TUPLE, qual, loc)

#define TENSOR_API_MAP_BASE(type, qual, loc) Std::tuple<TENSOR_API_TYPE_ALIAS(qual, type), type>,

#define TENSOR_API_GEN_BASE_MAP(qual, loc) TENSOR_API_USER_TYPES(TENSOR_API_MAP_BASE, qual, loc)

struct LocationTypeCombo {
    TENSOR_API_QUAL_LOC_PAIRS(TENSOR_API_GENERATE_ALL_TYPES)

    using locationType2Location = TupleMap<TENSOR_API_QUAL_LOC_PAIRS(TENSOR_API_PAIR_QUAL_LOC) Std::tuple<void, void>>;

    using locationType2Type = TupleMap<TENSOR_API_QUAL_LOC_PAIRS(TENSOR_API_GEN_BASE_MAP) Std::tuple<void, void>>;
};

template <typename T>
using GetAttributeLocation = typename LocationTypeCombo::locationType2Location::template Get<T>;

template <typename T>
using GetAttributeElementType = typename LocationTypeCombo::locationType2Type::template Get<T>;

} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_UTILS_LOCATION_IMPL_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
