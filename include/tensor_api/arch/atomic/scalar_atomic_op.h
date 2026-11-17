/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software distributed under the License is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

#if !defined(ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS)
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_ATOMIC_OP_H
#endif
/*!
 * \file scalar_atomic_op.h
 * \brief Scalar atomic operation interfaces.
 */
#ifndef INCLUDE_TENSOR_API_ARCH_ATOMIC_SCALAR_ATOMIC_OP_H
#define INCLUDE_TENSOR_API_ARCH_ATOMIC_SCALAR_ATOMIC_OP_H

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)

namespace asc {
namespace te {

/**
 * @brief Atomically adds a value to a Global Memory element.
 *
 * @tparam T Data type of the element.
 * @param address Global Memory address of the element.
 * @param val Value added to the element.
 * @return Original value of the element before the operation.
 */
template <typename T>
__aicore__ inline T atomic_add(__gm__ T* address, T val);

/**
 * @brief Atomically subtracts a value from a Global Memory element.
 *
 * @tparam T Data type of the element.
 * @param address Global Memory address of the element.
 * @param val Value subtracted from the element.
 * @return Original value of the element before the operation.
 */
template <typename T>
__aicore__ inline T atomic_sub(__gm__ T* address, T val);

/**
 * @brief Atomically computes the minimum of a Global Memory element and a value.
 *
 * @tparam T Data type of the element.
 * @param address Global Memory address of the element.
 * @param val Value compared with the element.
 * @return Original value of the element before the operation.
 */
template <typename T>
__aicore__ inline T atomic_min(__gm__ T* address, T val);

/**
 * @brief Atomically computes the maximum of a Global Memory element and a value.
 *
 * @tparam T Data type of the element.
 * @param address Global Memory address of the element.
 * @param val Value compared with the element.
 * @return Original value of the element before the operation.
 */
template <typename T>
__aicore__ inline T atomic_max(__gm__ T* address, T val);

/**
 * @brief Atomically computes the bitwise AND of a Global Memory element and a value.
 *
 * @tparam T Data type of the element.
 * @param address Global Memory address of the element.
 * @param val Value combined with the element.
 * @return Original value of the element before the operation.
 */
template <typename T>
__aicore__ inline T atomic_and(__gm__ T* address, T val);

/**
 * @brief Atomically computes the bitwise OR of a Global Memory element and a value.
 *
 * @tparam T Data type of the element.
 * @param address Global Memory address of the element.
 * @param val Value combined with the element.
 * @return Original value of the element before the operation.
 */
template <typename T>
__aicore__ inline T atomic_or(__gm__ T* address, T val);

/**
 * @brief Atomically computes the bitwise XOR of a Global Memory element and a value.
 *
 * @tparam T Data type of the element.
 * @param address Global Memory address of the element.
 * @param val Value combined with the element.
 * @return Original value of the element before the operation.
 */
template <typename T>
__aicore__ inline T atomic_xor(__gm__ T* address, T val);

/**
 * @brief Atomically adds a value to a Global Memory element with wraparound.
 *
 * @tparam T Data type of the element.
 * @param address Global Memory address of the element.
 * @param val Value added to the element.
 * @return Original value of the element before the operation.
 */
template <typename T>
__aicore__ inline T atomic_inc(__gm__ T* address, T val);

/**
 * @brief Atomically subtracts a value from a Global Memory element with wraparound.
 *
 * @tparam T Data type of the element.
 * @param address Global Memory address of the element.
 * @param val Value subtracted from the element.
 * @return Original value of the element before the operation.
 */
template <typename T>
__aicore__ inline T atomic_dec(__gm__ T* address, T val);

/**
 * @brief Atomically replaces a Global Memory element with a value.
 *
 * @tparam T Data type of the element.
 * @param address Global Memory address of the element.
 * @param val Value written to the element.
 * @return Original value of the element before the operation.
 */
template <typename T>
__aicore__ inline T atomic_exch(__gm__ T* address, T val);

/**
 * @brief Atomically compares and exchanges a Global Memory element.
 *
 * @tparam T Data type of the element.
 * @param address Global Memory address of the element.
 * @param compare Value compared with the element.
 * @param desired Value written when the element equals compare.
 * @return Original value of the element before the operation.
 */
template <typename T>
__aicore__ inline T atomic_cas(__gm__ T* address, T compare, T desired);

} // namespace te
} // namespace asc

#include "impl/tensor_api/arch/atomic/scalar_atomic_op_impl.h"
#endif

#endif // INCLUDE_TENSOR_API_ARCH_ATOMIC_SCALAR_ATOMIC_OP_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_ATOMIC_OP_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_SCALAR_ATOMIC_OP_H
#endif
