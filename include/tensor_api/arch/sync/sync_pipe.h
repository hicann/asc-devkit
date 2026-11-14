/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and limitations under the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SYNC_PIPE_H
#endif

/*!
 * ile sync_pipe.h
 * rief Intra-core pipeline synchronization interface.
 */
#ifndef INCLUDE_TENSOR_API_ARCH_SYNC_SYNC_PIPE_H
#define INCLUDE_TENSOR_API_ARCH_SYNC_SYNC_PIPE_H

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
namespace asc {
namespace te {

/**
 * @brief Blocks subsequent instructions of the selected pipeline until preceding instructions complete.
 *
 * @param pipe Pipeline to synchronize.
 */
__aicore__ inline void sync_pipe(pipe_t pipe);

} // namespace te
} // namespace asc

#include "impl/tensor_api/arch/sync/sync_pipe_impl.h"
#endif

#endif // INCLUDE_TENSOR_API_ARCH_SYNC_SYNC_PIPE_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SYNC_PIPE_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SYNC_PIPE_H
#endif
