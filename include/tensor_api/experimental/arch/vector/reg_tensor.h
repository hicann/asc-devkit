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
#define UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_TENSOR_H
#endif

/*!
 * \file reg_tensor.h
 * \brief Register tensor types and related type traits.
 */
#ifndef INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_TENSOR_H
#define INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_TENSOR_H

#include "tensor_api/utils/utils.h"
#include "impl/tensor_api/experimental/arch/vector/type_get.h"

namespace asc {
namespace te {
namespace experimental {

/**
 * @brief Holds vector register data together with its active-lane mask.
 * @tparam DataType Element type represented by the vector register.
 */
template <typename DataType>
struct reg_tensor {
    /** @brief Element type represented by the vector register. */
    using elem_type = DataType;

    /** @brief Architecture-specific vector register type for the element type. */
    using reg_type = typename detail::type_get<elem_type>::type;

    /** @brief Default-constructs a register tensor. */
    __simd_callee__ reg_tensor() = default;

    /**
     * @brief Constructs a register tensor with the specified active-lane mask.
     * @param maskValue Raw vector predicate used as the active-lane mask.
     */
    __simd_callee__ reg_tensor(const vector_bool& maskValue) : mask(maskValue) {}

    /**
     * @brief Sets the active-lane mask from a Boolean register tensor.
     * @param maskReg Register tensor whose register value supplies the mask.
     * @return A reference to this register tensor.
     */
    __simd_callee__ inline reg_tensor& with_mask(const reg_tensor<bool>& maskReg);

    /** @brief Underlying architecture-specific vector register. */
    reg_type reg;

    /** @brief Predicate selecting the active register lanes. */
    vector_bool mask;
};

/**
 * @brief Indicates whether a type is a reg_tensor specialization.
 * @tparam T Type to inspect.
 */
template <typename T>
struct is_reg_tensor : AscendC::Std::false_type {};

/** @brief Specialization identifying reg_tensor types. */
template <typename DataType>
struct is_reg_tensor<reg_tensor<DataType>> : AscendC::Std::true_type {};

/**
 * @brief Indicates whether a type, after removing cv-reference qualifiers, is a reg_tensor specialization.
 * @tparam T Type to inspect.
 */
template <typename T>
inline constexpr bool is_reg_tensor_v = is_reg_tensor<AscendC::Std::remove_cvref_t<T>>::value;

/**
 * @brief Holds a pair of register tensors with the same element type.
 * @tparam DataType Element type represented by both register tensors.
 */
template <typename DataType>
struct reg_pair {
    /** @brief First register tensor. */
    reg_tensor<DataType> first;

    /** @brief Second register tensor. */
    reg_tensor<DataType> second;
};

} // namespace experimental
} // namespace te
} // namespace asc

#if defined(__NPU_ARCH__) && (__NPU_ARCH__ == 3510)
#include "impl/tensor_api/experimental/arch/vector/reg_tensor_impl.h"
#endif

#endif // INCLUDE_TENSOR_API_EXPERIMENTAL_ARCH_VECTOR_REG_TENSOR_H

#if defined(UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_TENSOR_H)
#undef ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS
#undef UNDEF_ASCENDC_TENSOR_API_INCLUDE_COMPILER_INTERNAL_HEADERS_REG_TENSOR_H
#endif
