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
#define ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_PRINT_H
#endif

/*!
 * \file print.h
 * \brief Tensor API debug print helpers.
 */
#ifndef INCLUDE_TENSOR_API_UTILS_PRINT_H
#define INCLUDE_TENSOR_API_UTILS_PRINT_H

#include "impl/tensor_api/utils/print_impl.h"

namespace asc {
namespace te {

/**
 * @brief Prints a format string.
 * @param fmt : Format string to print.
 */
__aicore__ inline void print(__gm__ const char* fmt);

/**
 * @brief Prints values according to a format string.
 * @param fmt : Format string that controls the output.
 * @param arg0 : First value to print.
 * @param args : Remaining values to print.
 */
template <typename Arg0, typename... Args>
__aicore__ inline void print(__gm__ const char* fmt, Arg0&& arg0, Args&&... args);

/**
 * @brief Prints a scalar, tuple, layout, or tensor description.
 * @param value : Value to print.
 */
template <typename Value, typename Std::enable_if_t<!detail::is_print_string_v<Value>, int> Enable>
__aicore__ inline void print(const Value& value);

/**
 * @brief Prints a rank-2 layout as a logical-coordinate-to-physical-index table.
 * @param layout : Layout to print.
 */
template <typename Layout>
__aicore__ inline void print_layout(const Layout& layout);

/**
 * @brief Prints tensor elements in logical coordinate order. The supported memory locations are the intersection of
 *        the Tensor API input/output locations and the asc_dump memory locations available on the target product.
 *        For other memory locations, prints the tensor description and an unsupported-location message.
 * @param tensor : Tensor to print.
 * @param print_type : Whether to print the tensor pointer and layout before its elements.
 */
template <typename Tensor>
__aicore__ inline void print_tensor(const Tensor& tensor, bool print_type);

} // namespace te
} // namespace asc

#endif // INCLUDE_TENSOR_API_UTILS_PRINT_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_PRINT_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_ASCENDC_TENSOR_API_PRINT_H
#endif
