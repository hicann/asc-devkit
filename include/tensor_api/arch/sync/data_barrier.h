/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License"). You may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and limitations under the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_DATA_BARRIER_H
#endif

/*!
 * \file data_barrier.h
 * \brief Scalar memory access data barrier interface.
 */
#ifndef INCLUDE_TENSOR_API_ARCH_SYNC_DATA_BARRIER_H
#define INCLUDE_TENSOR_API_ARCH_SYNC_DATA_BARRIER_H

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
namespace asc {
namespace te {

/**
 * @brief Blocks subsequent instructions until previous memory accesses complete.
 *
 * @param scope Memory scope waited by the barrier.
 */
__aicore__ inline void sync_data_barrier(mem_dsb_t scope);

} // namespace te
} // namespace asc

#include "impl/tensor_api/arch/sync/data_barrier_impl.h"
#endif

#endif // INCLUDE_TENSOR_API_ARCH_SYNC_DATA_BARRIER_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_DATA_BARRIER_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_DATA_BARRIER_H
#endif
