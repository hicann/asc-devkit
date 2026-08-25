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
    "impl/tensor_api/arch/vector/utils/mask_utils.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file mask_utils.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_UTILS_MASK_UTILS_H
#define IMPL_TENSOR_API_ARCH_VECTOR_UTILS_MASK_UTILS_H

#include "impl/tensor_api/tensor/pointer_pattern.h"
#include "impl/tensor_api/tensor/tensor_impl.h"
#include "impl/tensor_api/arch/utils/arch_utils.h"

namespace AscendC {
namespace Te {
namespace Inst {

class CreateMask {
public:
    template <typename T, typename PatType>
    __simd_callee__ inline static vector_bool Run()
    {
        if constexpr (sizeof(T) == sizeof(uint32_t)) {
            return asc_create_mask_b32(PatType());
        } else if constexpr (sizeof(T) == sizeof(uint16_t)) {
            return asc_create_mask_b16(PatType());
        } else if constexpr (sizeof(T) == sizeof(uint8_t)) {
            return asc_create_mask_b8(PatType());
        } else {
            return vector_bool{};
        }
    }
};

class UpdateMask {
public:
    template <typename T, typename U>
    __simd_callee__ inline static vector_bool Run(U& value)
    {
        if constexpr (sizeof(T) == sizeof(uint32_t)) {
            return asc_update_mask_b32(value);
        } else if constexpr (sizeof(T) == sizeof(uint16_t)) {
            return asc_update_mask_b16(value);
        } else if constexpr (sizeof(T) == sizeof(uint8_t)) {
            return asc_update_mask_b8(value);
        } else {
            return vector_bool{};
        }
    }
};

} // namespace Inst
} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_VECTOR_UTILS_MASK_UTILS_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
