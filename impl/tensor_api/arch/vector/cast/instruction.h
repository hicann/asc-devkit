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
    "impl/tensor_api/arch/vector/cast/instruction.h is an internal header file and must not be used directly. Functions or variables defined in this file maybe removed in the future. Please use "#include "tensor_api/tensor.h"" and use public functions or variables defined in interface headers files."
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif

/*!
 * \file instruction.h
 * \brief
 */
#ifndef IMPL_TENSOR_API_ARCH_VECTOR_CAST_INSTRUCTION_H
#define IMPL_TENSOR_API_ARCH_VECTOR_CAST_INSTRUCTION_H

#include "impl/tensor_api/tensor/pointer_pattern.h"
#include "impl/tensor_api/tensor/tensor_impl.h"
#include "impl/tensor_api/arch/utils/arch_utils.h"

namespace AscendC {
namespace Te {
namespace Inst {

struct Cast {};

#define ASCENDC_ROUND_INST(CLASS_NAME, FUNC_NAME)                               \
    class CLASS_NAME {                                                          \
    public:                                                                     \
        template <typename T>                                                   \
        __simd_callee__ inline static void Run(T& dst, T src, vector_bool mask) \
        {                                                                       \
            if constexpr (CURRENT_ARCH_VERSION == ArchVersion::V3510) {         \
                FUNC_NAME(dst, src, mask);                                      \
            }                                                                   \
        }                                                                       \
    }

#include "impl/tensor_api/arch/vector/cast/instruction/rounding_ops.h"

#undef ASCENDC_ROUND_INST

#define ASCENDC_CAST_INST(CLASS_NAME, FUNC_NAME)                                \
    class CLASS_NAME {                                                          \
    public:                                                                     \
        template <typename T, typename U>                                       \
        __simd_callee__ inline static void Run(T& dst, U src, vector_bool mask) \
        {                                                                       \
            if constexpr (CURRENT_ARCH_VERSION == ArchVersion::V3510) {         \
                FUNC_NAME(dst, src, mask);                                      \
            }                                                                   \
        }                                                                       \
    }

#include "impl/tensor_api/arch/vector/cast/instruction/bfloat16_fp4x2.h"
#include "impl/tensor_api/arch/vector/cast/instruction/bfloat16_float.h"
#include "impl/tensor_api/arch/vector/cast/instruction/bfloat16_half.h"
#include "impl/tensor_api/arch/vector/cast/instruction/bfloat16_int.h"
#include "impl/tensor_api/arch/vector/cast/instruction/float_fp8.h"
#include "impl/tensor_api/arch/vector/cast/instruction/float_half.h"
#include "impl/tensor_api/arch/vector/cast/instruction/float_hif8.h"
#include "impl/tensor_api/arch/vector/cast/instruction/float_int.h"
#include "impl/tensor_api/arch/vector/cast/instruction/half_hif8.h"
#include "impl/tensor_api/arch/vector/cast/instruction/half_int.h"
#include "impl/tensor_api/arch/vector/cast/instruction/half_uint.h"
#include "impl/tensor_api/arch/vector/cast/instruction/int_int.h"
#include "impl/tensor_api/arch/vector/cast/instruction/int_uint.h"
#include "impl/tensor_api/arch/vector/cast/instruction/uint_uint.h"

#undef ASCENDC_CAST_INST

} // namespace Inst
} // namespace Te
} // namespace AscendC

#endif // IMPL_TENSOR_API_ARCH_VECTOR_CAST_INSTRUCTION_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC
#endif
