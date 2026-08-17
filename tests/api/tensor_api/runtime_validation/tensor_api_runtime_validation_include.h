/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef TESTS_API_TENSOR_API_RUNTIME_VALIDATION_TENSOR_API_RUNTIME_VALIDATION_INCLUDE_H
#define TESTS_API_TENSOR_API_RUNTIME_VALIDATION_TENSOR_API_RUNTIME_VALIDATION_INCLUDE_H

#include "tensor_api/stub/cce_stub.h"

namespace tensor_api_runtime_validation {

struct TrapException {};

[[noreturn]] inline void throw_trap_exception()
{
    throw TrapException{};
}

} // namespace tensor_api_runtime_validation

#ifdef trap
#error "trap must not be defined before the Tensor API runtime-validation test wrapper"
#endif

#ifdef IMPL_TENSOR_API_UTILS_NPU_DEBUG_ASSERT_H
#error "Tensor API headers must be included through the runtime-validation test wrapper"
#endif

#define trap() ::tensor_api_runtime_validation::throw_trap_exception()
#include "include/tensor_api/tensor.h"
#undef trap

#endif // TESTS_API_TENSOR_API_RUNTIME_VALIDATION_TENSOR_API_RUNTIME_VALIDATION_INCLUDE_H
