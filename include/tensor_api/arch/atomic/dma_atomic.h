/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and limitations under the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_DMA_ATOMIC_H
#endif

/*!
 * \file dma_atomic.h
 * \brief DMA atomic mode interfaces.
 */
#ifndef INCLUDE_TENSOR_API_ARCH_ATOMIC_DMA_ATOMIC_H
#define INCLUDE_TENSOR_API_ARCH_ATOMIC_DMA_ATOMIC_H

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
namespace asc {
namespace te {

/**
 * @brief Enables atomic addition for subsequent supported DMA transfers to GM.
 *
 * @tparam T Data type used by the subsequent atomic transfer.
 */
template <typename T>
__aicore__ inline void set_atomic_add();

/**
 * @brief Enables atomic maximum for subsequent supported DMA transfers to GM.
 *
 * @tparam T Data type used by the subsequent atomic transfer.
 */
template <typename T>
__aicore__ inline void set_atomic_max();

/**
 * @brief Enables atomic minimum for subsequent supported DMA transfers to GM.
 *
 * @tparam T Data type used by the subsequent atomic transfer.
 */
template <typename T>
__aicore__ inline void set_atomic_min();

/** @brief Disable DMA atomic mode. This does not wait for transfer completion. */
__aicore__ inline void disable_dma_atomic();

} // namespace te
} // namespace asc

#include "impl/tensor_api/arch/atomic/dma_atomic_impl.h"
#endif

#endif // INCLUDE_TENSOR_API_ARCH_ATOMIC_DMA_ATOMIC_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_DMA_ATOMIC_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_DMA_ATOMIC_H
#endif
