/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#ifndef TESTS_API_TENSOR_API_RUNTIME_VALIDATION_STUB_UTILS_DEBUG_ASC_PRINTF_H
#define TESTS_API_TENSOR_API_RUNTIME_VALIDATION_STUB_UTILS_DEBUG_ASC_PRINTF_H

namespace __asc_aicore {

template <typename... ArgTypes>
__aicore__ inline void printf_impl_assert(__gm__ const char*, ArgTypes&&...)
{}

} // namespace __asc_aicore

#endif // TESTS_API_TENSOR_API_RUNTIME_VALIDATION_STUB_UTILS_DEBUG_ASC_PRINTF_H
